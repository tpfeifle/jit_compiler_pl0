#include <pljit/lexer/Lexer.hpp>
#include "PljitFunction.hpp"
//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
int PljitFunction::compile() {
    // Compiling
    pljit_parser::Parser parser = pljit_parser::Parser(code);
    std::unique_ptr<pljit_parser::NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    if(!pt) {
        return -1;
    }
    astRoot = ast.analyzeParseTree(std::move(pt));
    if (!astRoot) {
        return -2;
    }

    // Optimizations
    pljit_ir::OptimizeDeadCode optimizeDeadCode = pljit_ir::OptimizeDeadCode();
    optimizeDeadCode.optimizeAST(*astRoot);
    pljit_ir::OptimizeConstPropagation optimizeConstPropagation = pljit_ir::OptimizeConstPropagation(ast.symbolTable);
    optimizeConstPropagation.optimizeAST(*astRoot);
    return 0;
}
//---------------------------------------------------------------------------
} // namespace pljit_function
//---------------------------------------------------------------------------