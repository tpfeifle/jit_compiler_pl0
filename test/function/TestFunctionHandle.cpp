#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <pljit/function/pljit.hpp>
//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
void funcWrapper(FunctionHandle functionHandle, unsigned a, unsigned b, const std::vector<int64_t>& expectedRes)
// Wrapper for the multi-thread tests that gets a list of the expectedResults and checks if the gotten result
// is in that list
{
    int64_t res = functionHandle(a, b);
    EXPECT_NE(std::find(expectedRes.begin(), expectedRes.end(), res), expectedRes.end());
}

//---------------------------------------------------------------------------
TEST(Evaluate, TestSimpleReturn) {
    Pljit jit;
    auto func = jit.registerFunction("BEGIN\n"
                                     "    RETURN 12\n"
                                     "END.\n");
    auto result = func();
    EXPECT_EQ(result, 12);
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestComplexCalculation) {
    Pljit jit;
    auto func = jit.registerFunction("BEGIN\n"
                                     "    RETURN 12 * (4 - 1 * 32 / 1) - 3\n"
                                     "END.\n");
    auto result = func();
    EXPECT_EQ(result, -339);
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestUsingVarAndConst) {
    Pljit jit;
    auto func = jit.registerFunction("VAR I, cost;\n"
                                     "CONST U = 12, R = 4;\n"
                                     "BEGIN\n"
                                     "    I := U / R;\n"
                                     "    cost := I * 51;\n"
                                     "    RETURN cost - 12\n"
                                     "END.\n");
    auto result = func();
    EXPECT_EQ(result, 141);
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestVarRedefinition) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM foo, final;\n"
                                     "VAR I, cost;\n"
                                     "CONST U = 12, R = 4;\n"
                                     "BEGIN\n"
                                     "    I := 200;\n"
                                     "    I := 200 * U + R;\n"
                                     "    I := I - foo;\n"
                                     "    cost := I * (final + I);\n"
                                     "    RETURN cost / R\n"
                                     "END.\n");
    auto result = func(3, 4);
    EXPECT_EQ(result, 1443601);
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestMissingParameterInCall) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM foo, final;\n"
                                     "VAR I, cost;\n"
                                     "CONST U = 12, R = 4;\n"
                                     "BEGIN\n"
                                     "    I := 200;\n"
                                     "    I := 200 * U + R;\n"
                                     "    I := I - foo;\n"
                                     "    cost := I * (final + I);\n"
                                     "    RETURN cost / R\n"
                                     "END.\n");
    testing::internal::CaptureStderr();
    func();
    std::string error = testing::internal::GetCapturedStderr();
    EXPECT_EQ(error, "Error: missing parameter for executing the function\n");
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestDivisionByZero) {
    Pljit jit;
    auto func = jit.registerFunction("BEGIN\n"
                                     "    RETURN 12/0\n"
                                     "END.\n");

    testing::internal::CaptureStderr();
    func();
    std::string error = testing::internal::GetCapturedStderr();
    EXPECT_EQ(error, "Division by zero error\n");
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestSyntaxError) {
    Pljit jit;
    //auto func = jit.registerFunction("?asd HELLO\n");
    auto func = jit.registerFunction("PARAM a, ? b, \n");
    testing::internal::CaptureStderr();
    func();
    std::string error = testing::internal::GetCapturedStderr();
    EXPECT_EQ(error, "0:8:Unexpected character\nPARAM a, ? b, \n        ^\n");
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestSemanticError) {
    Pljit jit;
    auto func = jit.registerFunction("VAR a;\n"
                                     "PARAM b;\n");

    testing::internal::CaptureStderr();
    func();
    std::string error = testing::internal::GetCapturedStderr();
    EXPECT_EQ(error, "1:0:Expected BEGIN keyword\n"
                     "PARAM b;\n"
                     "^~~~~\n");
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestParameters) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM U, R;\n"
                                     "VAR I, cost;\n"
                                     "BEGIN\n"
                                     "    I := U / R;\n"
                                     "    cost := I * 51;\n"
                                     "    RETURN cost - 12\n"
                                     "END.\n");
    auto result = func(20, 5);
    EXPECT_EQ(result, 192);
}
//---------------------------------------------------------------------------
TEST(Function, TestConcurrent) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM a, b;\n"
                                     "BEGIN\n"
                                     "    RETURN a + b\n"
                                     "END.\n");


    std::vector<std::thread> threads;
    std::vector<int64_t> expectedRes = {0, 4, 8, 12, 16};
    for (uint32_t value = 0; value < 5; ++value) {
        threads.emplace_back(funcWrapper, func, value, value * 3, expectedRes);
    }

    for (auto& thread : threads)
        thread.join();
}
//---------------------------------------------------------------------------
TEST(Function, TestConcurrentComplicated) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM a, foo;\n"
                                     "VAR b, c;\n"
                                     "CONST size = 3;\n"
                                     "BEGIN\n"
                                     " b := a * size - 12;\n"
                                     " c := b / 3 + (2 * size);"
                                     "    RETURN a - (b + c)\n"
                                     "END.\n");


    std::vector<std::thread> threads;
    std::vector<int64_t> expectedRes = {-83,-1226,7,1,-104,7,22,127,-26};
    std::vector<int64_t> parameters = {31, 412, 1, 3, 38, 1, -4, -39, 12};
    for (int64_t param: parameters) {
        threads.emplace_back(funcWrapper, func, param, 0, expectedRes);
    }

    for (auto& thread : threads)
        thread.join();
}
//---------------------------------------------------------------------------
TEST(Function, TestMultipleFunc) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM a, b;\n"
                                     "BEGIN\n"
                                     "    RETURN a + b\n"
                                     "END.\n");
    auto func2 = jit.registerFunction("PARAM a, b;\n"
                                      "BEGIN\n"
                                      "    RETURN 3* a + b\n"
                                      "END.\n");
    auto res = func(1, 10);
    auto res2 = func2(3, 5);
    EXPECT_EQ(res, 11);
    EXPECT_EQ(res2, 14);
}
//---------------------------------------------------------------------------
TEST(Function, TestComplicatedUnaryExpr) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM a, b; BEGIN RETURN -2*+a -20*b END.\n");
    auto res = func(3, 5);
    EXPECT_EQ(res, -106);
}
//---------------------------------------------------------------------------
} // namespace pljit_function
//---------------------------------------------------------------------------



















