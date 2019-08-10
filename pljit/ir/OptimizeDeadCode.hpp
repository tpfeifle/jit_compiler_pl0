#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>

namespace pljit_ir {
//---------------------------------------------------------------------------
/// A visitor to remove dead code for optimization
struct OptimizeDeadCode : public pljit_ast::ASTVisitor {
    /// visit each node of the abstract syntax tree recursively
    void visit(pljit_ast::FunctionAST& node) override;
    void visit(pljit_ast::AssignmentAST& node) override;
    void visit(pljit_ast::ReturnStatementAST& node) override;
    void visit(pljit_ast::BinaryOperationAST& node) override;
    void visit(pljit_ast::UnaryAST& node) override;
    void visit(pljit_ast::LiteralAST& node) override;
    void visit(pljit_ast::IdentifierAST& node) override;
};
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------