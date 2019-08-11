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
std::unique_ptr<FunctionAST> getAstRoot2(const std::string& codeText) { // TODO fix linker error correctly
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    pljit_parser::Parser parser(code);
    std::unique_ptr<pljit_parser::NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    if (!pt) {
        exit(-1);
    }
    SemanticAnalyzer ast = SemanticAnalyzer();
    std::unique_ptr<FunctionAST> astRoot = ast.analyzeParseTree(std::move(pt));
    return astRoot;
}
//---------------------------------------------------------------------------
std::string getDotOutput2(const std::unique_ptr<FunctionAST>& astRoot) { //TODO s.o.
    testing::internal::CaptureStdout();
    DotASTVisitor visitor = DotASTVisitor();
    visitor.visit(*astRoot);
    return testing::internal::GetCapturedStdout();
}
//---------------------------------------------------------------------------
TEST(Ir, TestDeadCode) {
    std::string codeText = "PARAM foo;\n"
                           "BEGIN\n"
                           "    RETURN 1;\n"
                           "    foo := 12\n"
                           "END.\n";
    auto astRoot = getAstRoot2(codeText);
    pljit_ir::OptimizeDeadCode optimizeDeadCode = pljit_ir::OptimizeDeadCode();
    optimizeDeadCode.visit(*astRoot);
    std::string output = getDotOutput2(astRoot);
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
