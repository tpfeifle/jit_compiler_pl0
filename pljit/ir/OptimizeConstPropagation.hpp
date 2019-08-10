#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>

namespace pljit_ir {
//---------------------------------------------------------------------------
struct OptimizeConstPropagation : public pljit_ast::ASTVisitor {
    unsigned node_count = 0;
public:
    /// visit each node of the abstract syntax tree recursively
    void visit(pljit_ast::FunctionAST& node) override;
    void visit(pljit_ast::AssignmentAST& node) override;
    void visit(pljit_ast::ReturnStatementAST& node) override;
    void visit(pljit_ast::BinaryOperationAST& node) override;
    void visit(pljit_ast::UnaryAST& node) override;
    void visit(pljit_ast::LiteralAST& node) override;
    void visit(pljit_ast::IdentifierAST& node) override;

    explicit OptimizeConstPropagation(std::unordered_map<std::string, int> constValues);
    std::unordered_map<std::string, int> constValues;
    std::unordered_map<int, int> expressionMapping;

};
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------