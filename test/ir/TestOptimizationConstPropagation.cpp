#include "../../pljit/ast/AST.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ast/DotASTVisitor.hpp>
#include <pljit/ir/OptimizeConstPropagation.hpp>
//---------------------------------------------------------------------------
using namespace pljit::ast;
using namespace std;
using namespace pljit::lexer;
using namespace pljit::parser;
//---------------------------------------------------------------------------
namespace pljit::ast {
//---------------------------------------------------------------------------
string getDotOutput3(const unique_ptr<FunctionAST>& astRoot) { //TODO s.o.
    testing::internal::CaptureStdout();
    DotASTVisitor visitor = DotASTVisitor();
    visitor.visit(*astRoot);
    return testing::internal::GetCapturedStdout();
}
//---------------------------------------------------------------------------
TEST(Ir, TestConstPropagation) {
    vector<string> codeText = {"PARAM foo, loft;\n",
                               "CONST temp = 1200;\n",
                               "BEGIN\n",
                               "    foo := 12 * temp;\n",
                               "    RETURN loft + ( 1 + 2)\n",
                               "END.\n"};
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    Parser parser(lexer);
    shared_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    if(!pt) {
        exit(-1);
    }
    AST ast = AST();
    unique_ptr<FunctionAST> astRoot = ast.analyzeParseTree(pt);
    std::unordered_map<std::string, int> constValues{};
    for(const auto& el: ast.symbolTable) {
        if(el.second.first.type == ast::Symbol::Type::Const) {
            constValues.insert({el.first, el.second.first.value});
        }
    }
    pljit::ir::OptimizeConstPropagation optConstPropagation = pljit::ir::OptimizeConstPropagation(constValues);
    optConstPropagation.visit(*astRoot);
    string output = getDotOutput3(astRoot);

    string expectedOutput = "AST0[label=\"Function\"];\n"
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
    assert(output == expectedOutput);
}
//---------------------------------------------------------------------------
} // namespace pljit::ast
//---------------------------------------------------------------------------
