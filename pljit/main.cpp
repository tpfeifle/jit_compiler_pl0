#include <iostream>
//#include <pljit/ir/OptimizeDeadCode.hpp>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "parser/PTNode.hpp"
#include "ast/AST.hpp"
#include "ast/DotASTVisitor.hpp"
#include "ir/Evaluate.hpp"

//---------------------------------------------------------------------------
using namespace std;
using namespace pljit;
//---------------------------------------------------------------------------
int main() {
    //cout << "use for experiments" << endl;

    /*SourceCode code = SourceCode("/Users/tim/VB_Shared/final/code.txt");
    if(code.readCode() == -1) {
        std::cout << "Not valid filename" << std::endl;
        return -1;
    }*/
    std::vector<string> textCode1 {"asdf := 12\n"};
    std::vector<string> textCode {"PARAM width, height, depth;\n",
                                  "VAR volume;\n",
                                  "CONST density = 2400;\n",
                                  "BEGIN\n",
                                  "volume := width * height * depth;\n",
                                  "RETURN density * volume\n",
                                  "END.\n"};
    SourceCode code = SourceCode(textCode);

    Lexer lexer = Lexer(code);
    // lexer.scanCode();

    Parser parser(lexer);
    std::shared_ptr<PTNode> pt = parser.parseFunctionDefinition();
    if(!pt) {
        std::cout << "Parser failed" << std::endl;
        return -1;
    }
    // const NonTerminalPTNode* derivedPtr= static_cast<const NonTerminalPTNode*>(pt.get());
    //DotOutput visitor = DotOutput();
    //visitor.visitPTNode(pt.get());
    //Evaluate evaluate = Evaluate();
    pljit::ast::AST ast = pljit::ast::AST();
    std::unique_ptr<pljit::ast::ASTNode> astRoot = ast.analyzeParseTree(pt);
    if(!astRoot) {
        std::cout << "Abstract Syntax Tree failed" << std::endl;
        return -1;
    }
    pljit::ast::DotASTVisitor visitorAST = pljit::ast::DotASTVisitor();
    visitorAST.visitASTNode(astRoot.get());

    //pljit::ir::OptimizeDeadCode optimizeDeadCode = pljit::ir::OptimizeDeadCode();
    //optimizeDeadCode.visitASTNode(astRoot.get());

    Evaluate evaluate = Evaluate(ast.symbolTable);

    // TODO where to pass the variables?
    evaluate.variables["width"] = 20;
    evaluate.variables["height"] = 10;
    evaluate.variables["depth"] = 5;

    astRoot->execute(evaluate);


    /*
    Optimizer optimizer;
    optimizer.optimize(ast);
    return Function(ast);*/
    int debug = 1;
}
//---------------------------------------------------------------------------
