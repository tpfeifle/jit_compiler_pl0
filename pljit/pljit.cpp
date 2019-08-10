#include <pljit/lexer/Lexer.hpp>
#include "pljit.hpp"

//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
FunctionHandle Pljit::registerFunction(std::vector<std::string> codeText) {
    SourceCode code = SourceCode(codeText);
    return FunctionHandle(code);
}
//---------------------------------------------------------------------------
template<typename... Sizes>
int FunctionHandle::operator()(Sizes... sizes) {
    std::vector<unsigned> args = {static_cast<unsigned>(sizes)...};

    if (!isCompiled) {
        this->compile();
        isCompiled = true;
    }

    Evaluate evaluate = Evaluate(ast.symbolTable);
    for (auto& el:ast.symbolTable) {
        if (el.second.first.type == ast::Symbol::Type::Param) {
            if (args.size() < el.second.second + 1) {
                std::cout << "Error: missing parameter for call" << std::endl;
                return -1; //TODO
            }
            evaluate.variables[el.first] = args[el.second.second];
        }
    }
    astRoot->execute(evaluate);
    return evaluate.variables["Return"];
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
} // namespace pljit
//---------------------------------------------------------------------------