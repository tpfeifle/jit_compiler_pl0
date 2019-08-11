#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>
#include <pljit/ast/ASTVisitor.hpp>
#include "Optimization.hpp"

namespace pljit_ir {
//---------------------------------------------------------------------------
/// Constant propagation optimization
class OptimizeConstPropagation : public Optimization {
    /// Identifier for the nodes in the expression mapping
    unsigned node_count = 0;

    /// visit each node of the abstract syntax tree recursively
    void visit(pljit_ast::FunctionAST& node) override;
    void visit(pljit_ast::AssignmentAST& node) override;
    void visit(pljit_ast::ReturnStatementAST& node) override;
    void visit(pljit_ast::BinaryOperationAST& node) override;
    void visit(pljit_ast::UnaryAST& node) override;
    void visit(pljit_ast::LiteralAST& node) override;
    void visit(pljit_ast::IdentifierAST& node) override;
public:
    /// Constructor
    explicit OptimizeConstPropagation(
            std::unordered_map<std::string, std::pair<pljit_ast::Symbol, unsigned>> symbolTable);

    /// Constant values
    std::unordered_map<std::string, int64_t> constValues;

    /// Mapping from expressions to constant value
    std::unordered_map<unsigned, int64_t> expressionMapping;

};
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------