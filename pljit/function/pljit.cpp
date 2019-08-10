#include <pljit/lexer/Lexer.hpp>
#include "pljit.hpp"

//---------------------------------------------------------------------------
namespace pljit::function {
//---------------------------------------------------------------------------
FunctionHandle Pljit::registerFunction(std::vector<std::string> codeText) {
    source::SourceCode code = source::SourceCode(codeText);
    return FunctionHandle(code);
}
//---------------------------------------------------------------------------
void FunctionHandle::compile() {
    lexer::Lexer lexer = lexer::Lexer(code);
    parser::Parser parser = parser::Parser(lexer);
    std::shared_ptr<parser::NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    if (!pt) {
        std::cout << "Parser failed" << std::endl;
        return;
    }
    astRoot = ast.analyzeParseTree(pt);
    if (!astRoot) {
        std::cout << "Abstract Syntax Tree failed" << std::endl;
        return;
    }

    ir::OptimizeDeadCode optimizeDeadCode = ir::OptimizeDeadCode();
    optimizeDeadCode.visit(*astRoot);

    std::unordered_map<std::string, int> constValues{};
    for (const auto& el: ast.symbolTable) {
        if (el.second.first.type == ast::Symbol::Type::Const) {
            constValues.insert({el.first, el.second.first.value});
        }
    }
    ir::OptimizeConstPropagation optimizeConstPropagation = ir::OptimizeConstPropagation(constValues);
    optimizeConstPropagation.visit(*astRoot);
}
} // namespace pljit::function
//---------------------------------------------------------------------------