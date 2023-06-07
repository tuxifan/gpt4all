#include "llmodel.h"
#include "macros.h"

#include <string>
#include <string_view>
#include <gtest/gtest.h>

namespace {
LLModel *model;
uint8_t *savedState;

TEST(GET_SUITE_NAME(CppAPITest_), ModelLoad) {
    LLModel::setImplementationsSearchPath("..");
    model = LLModel::construct(LLMODEL_TEST_MODEL, "auto");
    EXPECT_NE(model, nullptr) << "Implementation creation failed";
    EXPECT_FALSE(model->isModelLoaded());
    EXPECT_TRUE(model->loadModel(LLMODEL_TEST_MODEL)) << "Model load failed";
    EXPECT_TRUE(model->isModelLoaded());
}

TEST(GET_SUITE_NAME(CppAPITest_), Prompt) {
    static std::string response;

    auto prompt_cb = [] (int32_t) {
        return true;
    };
    auto response_cb = [] (int32_t, const std::string& token) {
        response += token;
        return response.size() < LLMODEL_EXPECTED_RESPONSE_LEN;
    };
    auto recalc_cb = [] (bool is_recalculating) {
        EXPECT_FALSE(is_recalculating);
        return true;
    };

    LLModel::PromptContext ctx;
    ctx.top_k = 0;
    ctx.top_p = 1.0f;
    ctx.temp = 0.6f;
    ctx.n_ctx = 2024;
    ctx.n_batch = 16;
    ctx.n_predict = LLMODEL_EXPECTED_RESPONSE_LEN;
    ctx.repeat_penalty = 1.10f;
    ctx.repeat_last_n = 64;

    model->prompt("Did you know?", prompt_cb, response_cb, recalc_cb, ctx);

    EXPECT_FALSE(response.empty()) << "Model didn't generate a response";
    if (response.size() >= LLMODEL_EXPECTED_RESPONSE_LEN) response.resize(LLMODEL_EXPECTED_RESPONSE_LEN);
    EXPECT_EQ(response, LLMODEL_EXPECTED_RESPONSE) << "Model didn't generate the expected response";
}

/*TEST(GET_SUITE_NAME(CppAPITest_), StateSave) {
    savedState = new uint8_t[model->stateSize()];
    EXPECT_GT(model->saveState(savedState), 0);
}

TEST(GET_SUITE_NAME(CppAPITest_), StateRestore) {
    EXPECT_GT(model->restoreState(savedState), 0);
    delete []savedState;
}*/

TEST(GET_SUITE_NAME(CppAPITest_), ModelUnload) {
    delete model;
}
}
