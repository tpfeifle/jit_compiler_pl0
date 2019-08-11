#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>
#include <pljit/ast/ASTVisitor.hpp>
//---------------------------------------------------------------------------
namespace pljit_ir {
//---------------------------------------------------------------------------
/// Abstract class representing an optimization pass
/// Allows its derived classes to only implement the for the optimization required visitor-functions,
/// as defaults are already provided

class Optimization : public pljit_ast::ASTVisitor {
    /// Defaults for the different visitor methods
    void visit(pljit_ast::FunctionAST& node) override;
    void visit(pljit_ast::AssignmentAST& node) override;
    void visit(pljit_ast::ReturnStatementAST& node) override;
    void visit(pljit_ast::BinaryOperationAST& node) override;
    void visit(pljit_ast::UnaryAST& node) override;
    void visit(pljit_ast::LiteralAST& node) override;
    void visit(pljit_ast::IdentifierAST& node) override;
public:
    /// Interface to optimize the ast
    void optimizeAST(pljit_ast::FunctionAST& node);
};
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------