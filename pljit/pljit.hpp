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
namespace pljit {
//---------------------------------------------------------------------------
class FunctionHandle {
    ast::AST ast = ast::AST(); // TODO is not allowed to be here. has to be part of Pljit directly
    std::unique_ptr<ast::FunctionAST> astRoot;
    SourceCode code;
    bool isCompiled = false;
public:
    explicit FunctionHandle(SourceCode code) : code(code) {}
    template<typename... Sizes>
    int operator()(Sizes... sizes);
    void compile();
};

class Pljit {
public:
    static FunctionHandle registerFunction(std::vector<std::string> codeText);
    //parser::Parser parser;
    std::unique_ptr<ast::ASTNode> ast;
};
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------