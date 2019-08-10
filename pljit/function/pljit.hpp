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
namespace pljit_function {
//---------------------------------------------------------------------------
class FunctionHandle {
    pljit_ast::AST ast = pljit_ast::AST(); // TODO is not allowed to be here. has to be part of Pljit directly
    std::unique_ptr<pljit_ast::FunctionAST> astRoot;
    pljit_source::SourceCode code;
    bool isCompiled = false;
public:
    explicit FunctionHandle(pljit_source::SourceCode code) : code(std::move(code)) {}
    template<typename... Sizes>
    int operator()(Sizes... sizes) {
        std::vector<unsigned> args = {static_cast<unsigned>(sizes)...};

        if (!isCompiled) {
            this->compile();
            isCompiled = true;
        }

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

class Pljit {
public:
    FunctionHandle registerFunction(std::vector<std::string> codeText);
    //pljit_parser::Parser parser;
    std::unique_ptr<pljit_ast::ASTNode> ast;
};
//---------------------------------------------------------------------------
} // namespace pljit_function
//---------------------------------------------------------------------------