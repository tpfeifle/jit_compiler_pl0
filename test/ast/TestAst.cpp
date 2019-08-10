#include "../../pljit/ast/AST.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ast/DotASTVisitor.hpp>
//---------------------------------------------------------------------------
using namespace pljit::ast;
using namespace std;
using namespace pljit::lexer;
using namespace pljit::parser;
//---------------------------------------------------------------------------
namespace pljit::ast {
//---------------------------------------------------------------------------
unique_ptr<FunctionAST> getAstRoot(const vector<string>& codeText) {
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    Parser parser(lexer);
    shared_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    if(!pt) {
        exit(-1);
    }
    AST ast = AST();
    unique_ptr<FunctionAST> astRoot = ast.analyzeParseTree(pt);
    return astRoot;
}
//---------------------------------------------------------------------------
string getDotOutput(const unique_ptr<FunctionAST>& astRoot) {
    testing::internal::CaptureStdout();
    DotASTVisitor visitor = DotASTVisitor();
    visitor.visit(*astRoot);
    return testing::internal::GetCapturedStdout();
}
//---------------------------------------------------------------------------
TEST(Ast, TestBrackets) {
    vector<string> codeText = {"BEGIN\n",
                               "    RETURN 12 * (3 - 2)\n",
                               "END.\n"};
    auto ast = getAstRoot(codeText);
    string output = getDotOutput(ast);
    string expectedOutput = "AST0[label=\"Function\"];\n"
                            "AST0 -> AST1\n"
                            "AST1[label=\"Return\"];\n"
                            "AST1 -> AST2\n"
                            "AST2[label=\"*\"];\n"
                            "AST2 -> AST3\n"
                            "AST3[label=\" 12 \"];\n"
                            "AST2 -> AST4\n"
                            "AST4[label=\"-\"];\n"
                            "AST4 -> AST5\n"
                            "AST5[label=\" 3 \"];\n"
                            "AST4 -> AST6\n"
                            "AST6[label=\" 2 \"];\n";
    assert(output == expectedOutput);
}
//---------------------------------------------------------------------------
TEST(Ast, TestSymbolTable) {
    vector<string> codeText = {"PARAM width, height;\n",
                               "VAR temp, foo;\n",
                               "CONST hello = 12, test = 2000;\n",
                               "BEGIN\n",
                               "    RETURN 12 * (3 - 2)\n",
                               "END.\n"};
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    Parser parser(lexer);
    shared_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();

    AST ast = AST();
    unique_ptr<FunctionAST> astRoot = ast.analyzeParseTree(pt);
    vector<pair<string, int>> expectedVariables = {{"width",  Symbol::Type::Param},
                                                   {"height", Symbol::Type::Param},
                                                   {"temp",   Symbol::Type::Var},
                                                   {"foo",    Symbol::Type::Var},
                                                   {"hello",  Symbol::Type::Const},
                                                   {"test",    Symbol::Type::Const}};
    for(auto var:expectedVariables) {
        assert(ast.symbolTable.find(var.first) != ast.symbolTable.end());
        assert(ast.symbolTable.at(var.first).first.type == var.second);
    }
}
//---------------------------------------------------------------------------
TEST(Ast, TestIdentifierDeclaredTwice) {
    vector<string> codeText = {"PARAM width, height;\n",
                               "VAR width;\n",
                               "BEGIN\n",
                               "    RETURN 1\n",
                               "END.\n"};
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    string error = testing::internal::GetCapturedStderr();
    string expectedError = "1:4:This identifier is already declared. Duplicate declarations are not allowed\n"
                           "VAR width;\n"
                           "    ^~~~~\n";
    assert(error == expectedError);
}
//---------------------------------------------------------------------------
TEST(Ast, TestUsingUndeclaredIdentifier) {
    vector<string> codeText = {"BEGIN\n",
                               "    RETURN width\n",
                               "END.\n"};
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    string error = testing::internal::GetCapturedStderr();
    string expectedError = "1:11:Using an undeclared identifier\n"
                           "    RETURN width\n"
                           "           ^~~~~\n";
    assert(error == expectedError);
}
//---------------------------------------------------------------------------
TEST(Ast, TestAssigningValueToConstant) {
    vector<string> codeText = {"CONST foo = 10;\n",
                               "BEGIN\n",
                               "    foo := 20;\n",
                               "    RETURN foo\n",
                               "END.\n"};
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    string error = testing::internal::GetCapturedStderr();
    string expectedError = "2:4:Assigning a value to a constant\n"
                           "    foo := 20;\n"
                           "    ^~~\n";
    assert(error == expectedError);

}
//---------------------------------------------------------------------------
TEST(Ast, TestUsingUnitializedVariable) {
    vector<string> codeText = {"VAR foo;\n",
                               "BEGIN\n",
                               "    RETURN foo\n",
                               "END.\n"};
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    string error = testing::internal::GetCapturedStderr();
    string expectedError = "2:11:Using an uninitialized identifier\n"
                           "    RETURN foo\n"
                           "           ^~~\n";
    assert(error == expectedError);

}
//---------------------------------------------------------------------------
TEST(Ast, TestMissingReturnStatement) {
    vector<string> codeText = {"PARAM foo;\n",
                               "BEGIN\n",
                               "    foo := 20\n"
                               "END.\n"};
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    string error = testing::internal::GetCapturedStderr();
    string expectedError = "Function has no return statement\n";
    assert(error == expectedError);

}
//---------------------------------------------------------------------------
} // namespace pljit::ast
//---------------------------------------------------------------------------
