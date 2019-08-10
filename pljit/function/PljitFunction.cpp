#include <pljit/lexer/Lexer.hpp>
#include "PljitFunction.hpp"

//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
void PljitFunction::compile() {
    pljit_lexer::Lexer lexer = pljit_lexer::Lexer(code);
    pljit_parser::Parser parser = pljit_parser::Parser(lexer);
    std::unique_ptr<pljit_parser::NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    if (!pt) {
        std::cout << "Parser failed" << std::endl;
        return;
    }
    astRoot = ast.analyzeParseTree(std::move(pt));
    if (!astRoot) {
        std::cout << "Abstract Syntax Tree failed" << std::endl;
        return;
    }

    pljit_ir::OptimizeDeadCode optimizeDeadCode = pljit_ir::OptimizeDeadCode();
    optimizeDeadCode.visit(*astRoot);

    std::unordered_map<std::string, int> constValues{};
    for (const auto& el: ast.symbolTable) {
        if (el.second.first.type == pljit_ast::Symbol::Type::Const) {
            constValues.insert({el.first, el.second.first.value});
        }
    }
    pljit_ir::OptimizeConstPropagation optimizeConstPropagation = pljit_ir::OptimizeConstPropagation(constValues);
    optimizeConstPropagation.visit(*astRoot);
}
} // namespace pljit_function
//---------------------------------------------------------------------------