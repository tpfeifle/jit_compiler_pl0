#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>
#include <pljit/ast/ASTVisitor.hpp>
#include "Optimization.hpp"

namespace pljit_ir {
//---------------------------------------------------------------------------
/// A visitor to remove dead code for optimization
class OptimizeDeadCode : public Optimization {
    /// visit each node of the abstract syntax tree recursively
    void visit(pljit_ast::FunctionAST& node) override;
};
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------