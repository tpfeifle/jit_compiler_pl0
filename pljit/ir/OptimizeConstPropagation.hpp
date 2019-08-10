#pragma once
//---------------------------------------------------------------------------
#include <pljit/ast/ASTNode.hpp>

namespace pljit::ir {
//---------------------------------------------------------------------------
struct OptimizeConstPropagation : public ast::ASTVisitor {
    unsigned node_count = 0;
public:
    /// visit each node of the abstract syntax tree recursively
    void visit(ast::FunctionAST& node) override;
    //void visit(StatementAST& node) override;
    void visit(ast::AssignmentAST& node) override;
    void visit(ast::ReturnStatementAST& node) override;
    //void visit(ExpressionAST& node) override;
    void visit(ast::BinaryOperationAST& node) override;
    void visit(ast::UnaryAST& node) override;
    void visit(ast::LiteralAST& node) override;
    void visit(ast::IdentifierAST& node) override;

    explicit OptimizeConstPropagation(std::unordered_map<std::string, int> constValues);
    std::unordered_map<std::string, int> constValues;
    std::unordered_map<int, int> expressionMapping;

};
/*
struct OptimizeConstPropagation2 : public ast::ASTVisitor {
    unsigned node_count = 0;
public:
    /// visit each node of the abstract syntax tree recursively
    void visit(ast::FunctionAST& node) override;
    //void visit(StatementAST& node) override;
    void visit(ast::AssignmentAST& node) override;
    void visit(ast::ReturnStatementAST& node) override;
    //void visit(ExpressionAST& node) override;
    void visit(ast::BinaryOperationAST& node) override;
    void visit(ast::UnaryAST& node) override;
    void visit(ast::LiteralAST& node) override;
    void visit(ast::IdentifierAST& node) override;

    explicit OptimizeConstPropagation2(std::unordered_map<int, int> expressionMapping, std::unordered_map<std::string, int> constValues);
    std::unordered_map<int, int> expressionMapping;
    std::unordered_map<std::string, int> constValues;

};*/
//---------------------------------------------------------------------------
} // namespace pljit::ir
//---------------------------------------------------------------------------