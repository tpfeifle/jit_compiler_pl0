#pragma once
//---------------------------------------------------------------------------
#include "ASTNode.hpp"

namespace pljit {
//---------------------------------------------------------------------------
namespace ast {
//---------------------------------------------------------------------------
/// A visitor for the abstract syntax tree outputting the tree in dot formmat
struct DotASTVisitor: public VisitorAST {
    /// visit each node of the abstract syntax tree recursively
    void visitASTNode(ASTNode* node);
};
//---------------------------------------------------------------------------
} //namespace ast
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------