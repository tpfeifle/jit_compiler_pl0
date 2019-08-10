#include <gtest/gtest.h>
#include <vector>
#include <pljit/function/pljit.hpp>
//---------------------------------------------------------------------------
using namespace pljit_ast;
using namespace std;
using namespace pljit_function;
using namespace pljit_lexer;
using namespace pljit_parser;
//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
TEST(Evaluate, TestSimpleReturn) {
    Pljit jit;
    auto func = jit.registerFunction({
                                             "BEGIN\n",
                                             "    RETURN 12\n",
                                             "END.\n"});
    auto result = func();
    assert(result == 12);
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestComplexCalculation) {
    Pljit jit;
    auto func = jit.registerFunction({
                                             "BEGIN\n",
                                             "    RETURN 12 * (4 - 1 * 32 / 1) - 3\n",
                                             "END.\n"});
    auto result = func();
    assert(result == -339);
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestUsingVarAndConst) {
    Pljit jit;
    auto func = jit.registerFunction({
                                             "VAR I, cost;\n",
                                             "CONST U = 12, R = 4;\n",
                                             "BEGIN\n",
                                             "    I := U / R;\n",
                                             "    cost := I * 51;\n",
                                             "    RETURN cost - 12\n",
                                             "END.\n"});
    auto result = func();
    assert(result == 141);
}
//---------------------------------------------------------------------------
TEST(Evaluate, TestParameters) {
    Pljit jit;
    auto func = jit.registerFunction({
                                             "PARAM U, R;\n",
                                             "VAR I, cost;\n",
                                             "BEGIN\n",
                                             "    I := U / R;\n",
                                             "    cost := I * 51;\n",
                                             "    RETURN cost - 12\n",
                                             "END.\n"});
    auto result = func(20, 5);
    assert(result == 192);
}
//---------------------------------------------------------------------------
} // namespace pljit_ast
//---------------------------------------------------------------------------
