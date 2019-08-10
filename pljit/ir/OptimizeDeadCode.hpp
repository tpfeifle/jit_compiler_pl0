#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>

namespace pljit::ir {
//---------------------------------------------------------------------------
/// A visitor to remove dead code for optimization
struct OptimizeDeadCode : public ast::ASTVisitor {
    /// visit each node of the abstract syntax tree recursively
    void visit(ast::FunctionAST& node) override;
    void visit(ast::AssignmentAST& node) override;
    void visit(ast::ReturnStatementAST& node) override;
    void visit(ast::BinaryOperationAST& node) override;
    void visit(ast::UnaryAST& node) override;
    void visit(ast::LiteralAST& node) override;
    void visit(ast::IdentifierAST& node) override;
};
//---------------------------------------------------------------------------
} // namespace pljit::ir
//---------------------------------------------------------------------------