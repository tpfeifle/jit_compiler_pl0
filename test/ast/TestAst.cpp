#include "pljit/ast/SemanticAnalyzer.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ast/DotASTVisitor.hpp>
//---------------------------------------------------------------------------
using namespace pljit_lexer;
using namespace pljit_parser;
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
std::unique_ptr<FunctionAST> getAstRoot(const std::string& codeText) {
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Parser parser(code);
    std::unique_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    if (!pt) {
        exit(-1);
    }
    SemanticAnalyzer ast = SemanticAnalyzer();
    std::unique_ptr<FunctionAST> astRoot = ast.analyzeParseTree(std::move(pt));
    return astRoot;
}
//---------------------------------------------------------------------------
std::string getDotOutput(const std::unique_ptr<FunctionAST>& astRoot) {
    testing::internal::CaptureStdout();
    DotASTVisitor visitor = DotASTVisitor();
    visitor.visit(*astRoot);
    return testing::internal::GetCapturedStdout();
}
//---------------------------------------------------------------------------
TEST(Ast, TestBrackets) {
    std::string codeText = "BEGIN\n"
                      "    RETURN 12 * (3 - 2)\n"
                      "END.\n";
    auto ast = getAstRoot(codeText);
    std::string output = getDotOutput(ast);
    std::string expectedOutput = "AST0[label=\"Function\"];\n"
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
    std::string codeText = "PARAM width, height;\n"
                      "VAR temp, foo;\n"
                      "CONST hello = 12, test = 2000;\n"
                      "BEGIN\n"
                      "    RETURN 12 * (3 - 2)\n"
                      "END.\n";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Parser parser(code);
    std::unique_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();

    SemanticAnalyzer ast = SemanticAnalyzer();
    std::unique_ptr<FunctionAST> astRoot = ast.analyzeParseTree(std::move(pt));
    std::vector<std::pair<std::string, int>> expectedVariables = {{"width",  Symbol::Type::Param},
                                                   {"height", Symbol::Type::Param},
                                                   {"temp",   Symbol::Type::Var},
                                                   {"foo",    Symbol::Type::Var},
                                                   {"hello",  Symbol::Type::Const},
                                                   {"test",   Symbol::Type::Const}};
    for (auto var:expectedVariables) {
        assert(ast.symbolTable.find(var.first) != ast.symbolTable.end());
        assert(ast.symbolTable.at(var.first).first.type == var.second);
    }
}
//---------------------------------------------------------------------------
TEST(Ast, TestIdentifierDeclaredTwice) {
    std::string codeText = "PARAM width, height;\n"
                      "VAR width;\n"
                      "BEGIN\n"
                      "    RETURN 1\n"
                      "END.\n";
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    std::string error = testing::internal::GetCapturedStderr();
    std::string expectedError = "1:4:This identifier is already declared. Duplicate declarations are not allowed\n"
                           "VAR width;\n"
                           "    ^~~~~\n";
    assert(error == expectedError);
}
//---------------------------------------------------------------------------
TEST(Ast, TestUsingUndeclaredIdentifier) {
    std::string codeText = "BEGIN\n"
                      "    RETURN width\n"
                      "END.\n";
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    std::string error = testing::internal::GetCapturedStderr();
    std::string expectedError = "1:11:Using an undeclared identifier\n"
                           "    RETURN width\n"
                           "           ^~~~~\n";
    assert(error == expectedError);
}
//---------------------------------------------------------------------------
TEST(Ast, TestAssigningValueToConstant) {
    std::string codeText = "CONST foo = 10;\n"
                      "BEGIN\n"
                      "    foo := 20;\n"
                      "    RETURN foo\n"
                      "END.\n";
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    std::string error = testing::internal::GetCapturedStderr();
    std::string expectedError = "2:4:Assigning a value to a constant\n"
                           "    foo := 20;\n"
                           "    ^~~\n";
    assert(error == expectedError);

}
//---------------------------------------------------------------------------
TEST(Ast, TestUsingUnitializedVariable) {
    std::string codeText = "VAR foo;\n"
                      "BEGIN\n"
                      "    RETURN foo\n"
                      "END.\n";
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    std::string error = testing::internal::GetCapturedStderr();
    std::string expectedError = "2:11:Using an uninitialized identifier\n"
                           "    RETURN foo\n"
                           "           ^~~\n";
    assert(error == expectedError);

}
//---------------------------------------------------------------------------
TEST(Ast, TestMissingReturnStatement) {
    std::string codeText = "PARAM foo;\n"
                      "BEGIN\n"
                      "    foo := 20\n"
                      "END.\n";
    testing::internal::CaptureStderr();
    auto ast = getAstRoot(codeText);
    std::string error = testing::internal::GetCapturedStderr();
    std::string expectedError = "Function has no return statement\n";
    assert(error == expectedError);

}
//---------------------------------------------------------------------------
} // namespace pljit_ast
//---------------------------------------------------------------------------
