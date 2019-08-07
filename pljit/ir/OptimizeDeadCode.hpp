#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>

namespace pljit::ir {
//---------------------------------------------------------------------------
/// A visitor to remove dead code for optimization
struct OptimizeDeadCode : ast::VisitorAST {
    /// visit each node of the abstract syntax tree recursively
    void visitASTNode(ast::ASTNode* node);
};

//---------------------------------------------------------------------------
} // namespace pljit::ir
//---------------------------------------------------------------------------