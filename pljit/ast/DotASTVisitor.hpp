#pragma once
//---------------------------------------------------------------------------
#include "ASTNode.hpp"

namespace pljit_ast {
//---------------------------------------------------------------------------
/// A visitor for the abstract syntax tree outputting the tree in dot formmat
struct DotASTVisitor : public ASTVisitor {
    unsigned node_count = 0;
public:
    /// visit each node of the abstract syntax tree recursively
    void visit(FunctionAST& node) override;
    void visit(AssignmentAST& node) override;
    void visit(ReturnStatementAST& node) override;
    void visit(BinaryOperationAST& node) override;
    void visit(UnaryAST& node) override;
    void visit(LiteralAST& node) override;
    void visit(IdentifierAST& node) override;

};
//---------------------------------------------------------------------------
} // namespace pljit_ast
//---------------------------------------------------------------------------