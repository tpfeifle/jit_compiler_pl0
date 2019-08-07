#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>

namespace pljit::ir {
//---------------------------------------------------------------------------
struct OptimizeConstPropagation : ast::VisitorAST {
    /// visit each node of the abstract syntax tree recursively
    void visitASTNode(ast::ASTNode* node);

    std::unordered_map<std::string, int> constValues;
    std::unordered_map<ast::ExpressionAST, int> expressionMapping;
};
//---------------------------------------------------------------------------
} // namespace pljit::ir
//---------------------------------------------------------------------------