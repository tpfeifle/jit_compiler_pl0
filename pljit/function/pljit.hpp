#pragma once

#include <string>
#include <pljit/source/SourceCode.hpp>
#include <pljit/ast/ASTNode.hpp>
#include <pljit/ast/AST.hpp>
#include <pljit/ir/OptimizeConstPropagation.hpp>
#include <pljit/ir/OptimizeDeadCode.hpp>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
//---------------------------------------------------------------------------
namespace pljit::function {
//---------------------------------------------------------------------------
class FunctionHandle {
    ast::AST ast = ast::AST(); // TODO is not allowed to be here. has to be part of Pljit directly
    std::unique_ptr<ast::FunctionAST> astRoot;
    source::SourceCode code;
    bool isCompiled = false;
public:
    explicit FunctionHandle(source::SourceCode code) : code(std::move(code)) {}
    template<typename... Sizes>
    int operator()(Sizes... sizes) {
        std::vector<unsigned> args = {static_cast<unsigned>(sizes)...};

        if (!isCompiled) {
            this->compile();
            isCompiled = true;
        }

        ir::Evaluate evaluate = ir::Evaluate(ast.symbolTable);
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
    void compile();
};

class Pljit {
public:
    FunctionHandle registerFunction(std::vector<std::string> codeText);
    //parser::Parser parser;
    std::unique_ptr<ast::ASTNode> ast;
};
//---------------------------------------------------------------------------
} // namespace pljit::function
//---------------------------------------------------------------------------