#include "pljit/ast/SemanticAnalyzer.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ast/DotASTVisitor.hpp>
#include <pljit/ir/OptimizeDeadCode.hpp>
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
TEST(Ir, TestDeadCode) {
    std::string codeText = "PARAM foo;\n"
                           "BEGIN\n"
                           "    RETURN 1;\n"
                           "    foo := 12\n"
                           "END.\n";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    pljit_parser::Parser parser(code);
    std::unique_ptr<pljit_parser::NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    ASSERT_NE(pt, nullptr);
    SemanticAnalyzer ast = SemanticAnalyzer();
    std::unique_ptr<FunctionAST> astRoot = ast.analyzeParseTree(std::move(pt));
    ASSERT_NE(astRoot, nullptr);
    // Optimize
    pljit_ir::OptimizeDeadCode optimizeDeadCode = pljit_ir::OptimizeDeadCode();
    optimizeDeadCode.optimizeAST(*astRoot);

    // Compare output
    testing::internal::CaptureStdout();
    DotASTVisitor visitor = DotASTVisitor();
    visitor.visit(*astRoot);
    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = "AST0[label=\"Function\"];\n"
                                 "AST0 -> AST1\n"
                                 "AST1[label=\"Return\"];\n"
                                 "AST1 -> AST2\n"
                                 "AST2[label=\" 1 \"];\n";
    assert(output == expectedOutput);
}
//---------------------------------------------------------------------------
} // namespace pljit_ast
//---------------------------------------------------------------------------
