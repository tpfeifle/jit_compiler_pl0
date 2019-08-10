#include "../../pljit/ast/AST.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ast/DotASTVisitor.hpp>
#include <pljit/ir/OptimizeDeadCode.hpp>
//---------------------------------------------------------------------------
using namespace pljit::ast;
using namespace std;
using namespace pljit::lexer;
using namespace pljit::parser;
//---------------------------------------------------------------------------
namespace pljit::ast {
//---------------------------------------------------------------------------
unique_ptr<FunctionAST> getAstRoot2(const vector<string>& codeText) { // TODO fix linker error correctly
    SourceCode code = SourceCode(codeText);
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
string getDotOutput2(const unique_ptr<FunctionAST>& astRoot) { //TODO s.o.
    testing::internal::CaptureStdout();
    DotASTVisitor visitor = DotASTVisitor();
    visitor.visit(*astRoot);
    return testing::internal::GetCapturedStdout();
}
//---------------------------------------------------------------------------
TEST(Ast, TestDeadCode) {
    vector<string> codeText = {"PARAM foo;\n",
                               "BEGIN\n",
                               "    RETURN 1;\n",
                               "    foo := 12\n",
                               "END.\n"};
    auto astRoot = getAstRoot2(codeText);
    pljit::ir::OptimizeDeadCode optimizeDeadCode = pljit::ir::OptimizeDeadCode();
    optimizeDeadCode.visit(*astRoot);
    string output = getDotOutput2(astRoot);
    string expectedOutput = "AST0[label=\"Function\"];\n"
                            "AST0 -> AST1\n"
                            "AST1[label=\"Return\"];\n"
                            "AST1 -> AST2\n"
                            "AST2[label=\" 1 \"];\n";
    assert(output == expectedOutput);
}
//---------------------------------------------------------------------------
} // namespace pljit::ast
//---------------------------------------------------------------------------
