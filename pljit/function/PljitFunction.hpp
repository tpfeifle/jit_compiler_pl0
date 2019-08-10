#pragma once

#include <string>
#include <pljit/source/SourceCode.hpp>
#include <pljit/ast/ASTNode.hpp>
#include <pljit/ast/SemanticAnalyzer.hpp>
#include <pljit/ir/OptimizeConstPropagation.hpp>
#include <pljit/ir/OptimizeDeadCode.hpp>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
#include <mutex>
//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------

class PljitFunction {
    pljit_ast::SemanticAnalyzer ast = pljit_ast::SemanticAnalyzer();
    std::unique_ptr<pljit_ast::FunctionAST> astRoot;
    pljit_source::SourceCode code;
    bool isCompiled = false;
    std::mutex compileMutex;
public:
    explicit PljitFunction(pljit_source::SourceCode code) : code(std::move(code)) {}
    template<typename... Sizes>
    int execute(Sizes... sizes) {
        std::vector<unsigned> args = {static_cast<unsigned>(sizes)...};
        compileMutex.lock();
        if (!isCompiled) {
            this->compile();
            isCompiled = true;
        }
        compileMutex.unlock();

        pljit_ir::Evaluate evaluate = pljit_ir::Evaluate(ast.symbolTable);
        for (auto& el:ast.symbolTable) {
            if (el.second.first.type == pljit_ast::Symbol::Type::Param) {
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
    void compile();
};
//---------------------------------------------------------------------------
} // namespace pljit_function
//---------------------------------------------------------------------------