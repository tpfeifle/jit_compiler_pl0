#include "pljit/ast/SemanticAnalyzer.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ast/DotASTVisitor.hpp>
#include <pljit/ir/OptimizeConstPropagation.hpp>
//---------------------------------------------------------------------------
namespace pljit_ir {
//---------------------------------------------------------------------------
std::string getDotOutput(const std::string& codeText) {
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    pljit_parser::Parser parser(code);
    std::unique_ptr<pljit_parser::NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    EXPECT_NE(pt, nullptr);
    pljit_ast::SemanticAnalyzer ast = pljit_ast::SemanticAnalyzer();
    std::unique_ptr<pljit_ast::FunctionAST> astRoot = ast.analyzeParseTree(std::move(pt));
    EXPECT_NE(astRoot, nullptr);
    pljit_ir::OptimizeConstPropagation optConstPropagation = pljit_ir::OptimizeConstPropagation(ast.symbolTable);
    optConstPropagation.optimizeAST(*astRoot);
    testing::internal::CaptureStdout();
    pljit_ast::DotASTVisitor visitor = pljit_ast::DotASTVisitor();
    visitor.visit(*astRoot);
    return testing::internal::GetCapturedStdout();
}
//---------------------------------------------------------------------------
TEST(Ir, TestConstPropagation) {
    std::string codeText = "PARAM foo, loft;\n"
                           "CONST temp = 1200;\n"
                           "BEGIN\n"
                           "    foo := 12 * temp;\n"
                           "    RETURN loft + ( 1 + 2)\n"
                           "END.\n";
    std::string output = getDotOutput(codeText);
    std::string expectedOutput = "AST0[label=\"Function\"];\n"
                                 "AST0 -> AST1\n"
                                 "AST1[label=\"Assignment\"];\n"
                                 "AST1 -> AST2\n"
                                 "AST2[label=\" foo \"];\n"
                                 "AST1 -> AST3\n"
                                 "AST3[label=\" 14400 \"];\n"
                                 "AST0 -> AST4\n"
                                 "AST4[label=\"Return\"];\n"
                                 "AST4 -> AST5\n"
                                 "AST5[label=\"+\"];\n"
                                 "AST5 -> AST6\n"
                                 "AST6[label=\" loft \"];\n"
                                 "AST5 -> AST7\n"
                                 "AST7[label=\" 3 \"];\n";
    EXPECT_EQ(output, expectedOutput);
}
//---------------------------------------------------------------------------
TEST(Ir, TestConstPropagationOverwrite) {
    std::string codeText = "PARAM foo, var;\n"
                           "CONST temp = 1200;\n"
                           "BEGIN\n"
                           "    foo := 10;\n"
                           "    foo := foo + 1;\n"
                           "    foo := foo * temp;\n"
                           "    foo := foo * var + temp * 2;\n"
                           "    RETURN foo\n"
                           "END.\n";
    std::string output = getDotOutput(codeText);
    std::string expectedOutput = "AST0[label=\"Function\"];\n"
                                 "AST0 -> AST1\n"
                                 "AST1[label=\"Assignment\"];\n"
                                 "AST1 -> AST2\n"
                                 "AST2[label=\" foo \"];\n"
                                 "AST1 -> AST3\n"
                                 "AST3[label=\" 10 \"];\n"
                                 "AST0 -> AST4\n"
                                 "AST4[label=\"Assignment\"];\n"
                                 "AST4 -> AST5\n"
                                 "AST5[label=\" foo \"];\n"
                                 "AST4 -> AST6\n"
                                 "AST6[label=\" 11 \"];\n"
                                 "AST0 -> AST7\n"
                                 "AST7[label=\"Assignment\"];\n"
                                 "AST7 -> AST8\n"
                                 "AST8[label=\" foo \"];\n"
                                 "AST7 -> AST9\n"
                                 "AST9[label=\" 13200 \"];\n"
                                 "AST0 -> AST10\n"
                                 "AST10[label=\"Assignment\"];\n"
                                 "AST10 -> AST11\n"
                                 "AST11[label=\" foo \"];\n"
                                 "AST10 -> AST12\n"
                                 "AST12[label=\"+\"];\n"
                                 "AST12 -> AST13\n"
                                 "AST13[label=\"*\"];\n"
                                 "AST13 -> AST14\n"
                                 "AST14[label=\" 13200 \"];\n"
                                 "AST13 -> AST15\n"
                                 "AST15[label=\" var \"];\n"
                                 "AST12 -> AST16\n"
                                 "AST16[label=\" 2400 \"];\n"
                                 "AST0 -> AST17\n"
                                 "AST17[label=\"Return\"];\n"
                                 "AST17 -> AST18\n"
                                 "AST18[label=\" foo \"];\n";
    EXPECT_EQ(output, expectedOutput);
}
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------
