#include <iostream>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "parser/PTNode.hpp"
#include "ast/AST.hpp"

//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
int main() {
    cout << "use for experiments" << endl;

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
    DotOutput visitor = DotOutput();
    visitor.visitPTNode(pt.get());
    AST ast = AST();
    std::unique_ptr<ASTNode> astRoot = ast.analyzeParseTree(pt);
    if(!astRoot) {
        std::cout << "Abstract Syntax Tree failed" << std::endl;
        return -1;
    }
    /*
    Optimizer optimizer;
    optimizer.optimize(ast);
    return Function(ast);*/
}
//---------------------------------------------------------------------------
