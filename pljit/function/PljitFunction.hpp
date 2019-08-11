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
/// Function containg the AST for the specified PL/0 code
class PljitFunction {
    /// Semantic Analyzer used for the ast extraction
    pljit_ast::SemanticAnalyzer ast = pljit_ast::SemanticAnalyzer();

    /// Pointer to the root of the AST
    std::unique_ptr<pljit_ast::FunctionAST> astRoot;

    /// The provided PL/0 code
    pljit_source::SourceCode code;

    /// Flag whether or not the function is already compiled
    bool isCompiled = false;

    /// Mutex to enable access from multiple threads to compile the function
    std::mutex compileMutex; // TODO check if compileMutex enough or runMutex needed
public:
    /// Constructor
    explicit PljitFunction(pljit_source::SourceCode code) : code(std::move(code)) {}

    /// Executes the function
    template<typename... Params>
    int execute(Params... params) {
        std::vector<unsigned> args = {static_cast<unsigned>(params)...};
        compileMutex.lock();
        if (!isCompiled) {
            this->compile();
            isCompiled = true;
        }
        compileMutex.unlock();

        // read the provided parameters
        pljit_ir::EvalContext evaluate = pljit_ir::EvalContext(ast.symbolTable);
        for (auto& el:ast.symbolTable) {
            if (el.second.first.type == pljit_ast::Symbol::Type::Param) {
                if (args.size() < el.second.second + 1) {
                    std::cerr << "Error: missing parameter for executing the function" << std::endl;
                    return 0;
                }
                evaluate.parameters[el.first] = args[el.second.second];
            }
        }
        astRoot->execute(evaluate);
        return evaluate.parameters["Return"];
    }

    /// Compiles the function
    void compile();
};
//---------------------------------------------------------------------------
} // namespace pljit_function
//---------------------------------------------------------------------------