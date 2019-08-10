#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <pljit/ir/Evaluate.hpp>
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
struct FunctionAST;

struct StatementAST;
struct AssignmentAST;
struct ReturnStatementAST;

struct ExpressionAST;
struct LiteralAST;
struct IdentifierAST;
struct UnaryAST;
struct BinaryOperationAST;


class ASTVisitor {
public:
    virtual void visit(FunctionAST& node) = 0;
    virtual void visit(AssignmentAST& node) = 0;
    virtual void visit(ReturnStatementAST& node) = 0;
    virtual void visit(LiteralAST& node) = 0;
    virtual void visit(IdentifierAST& node) = 0;
    virtual void visit(UnaryAST& node) = 0;
    virtual void visit(BinaryOperationAST& node) = 0;
};

class ASTNode {
public:
    /// All possible types of Nodes
    enum class Type {
        AssignmentStatement,
        ReturnStatement,
        BinaryExpression,
        UnaryExpression,
        LiteralExpression,
        IdentifierExpression,
        Function
    };

    virtual void accept(ASTVisitor& v) = 0;

    explicit ASTNode(const Type type) : type(type) {}

    virtual int64_t execute(pljit_ir::Evaluate& evaluate) = 0;

    [[nodiscard]] Type getType() const; // adding zero means it is a "pure function"

    virtual ~ASTNode() = default; //public virtual destructor
private:
    Type type;
};


struct StatementAST : public ASTNode {
    explicit StatementAST(Type type) : ASTNode(type) {};
    ~StatementAST() override = default;
};

struct ExpressionAST : ASTNode {
    explicit ExpressionAST(const Type type) : ASTNode(type) {};
    ~ExpressionAST() override = default;
};

//---------------------------------------------------------------------------
struct FunctionAST : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> children;

    explicit FunctionAST(std::vector<std::unique_ptr<ASTNode>> children) : ASTNode(ASTNode::Type::Function),
                                                                           children(std::move(children)) {};

    void accept(ASTVisitor& v) override;
    int64_t execute(pljit_ir::Evaluate& evaluate) override;
};

//---------------------------------------------------------------------------
struct LiteralAST : ExpressionAST { // TODO: Idea use "LiteralAST : ASTNode" and then the types of ASTNode contain all possible types directly
    int64_t value;

    explicit LiteralAST(int64_t value) : ExpressionAST(ASTNode::Type::LiteralExpression), value(value) {};
    void accept(ASTVisitor& v) override;
    int64_t execute(pljit_ir::Evaluate& evaluate) override;
};

//---------------------------------------------------------------------------
struct UnaryAST : ExpressionAST {
    std::unique_ptr<ASTNode> child;
    enum SignType {
        Plus,
        Minus
    };
    SignType sign;

    explicit UnaryAST(std::unique_ptr<ASTNode> child, SignType sign)
            : ExpressionAST(Type::UnaryExpression), child(std::move(child)), sign(sign) {};
    void accept(ASTVisitor& v) override;
    int64_t execute(pljit_ir::Evaluate& evaluate) override;
};

//---------------------------------------------------------------------------
struct IdentifierAST : ExpressionAST {
    std::string identifier;

    explicit IdentifierAST(std::string identifier) : ExpressionAST(Type::IdentifierExpression),
                                                     identifier(std::move(identifier)) {};
    void accept(ASTVisitor& v) override;
    int64_t execute(pljit_ir::Evaluate& evaluate) override;
};
//---------------------------------------------------------------------------
struct BinaryOperationAST : ExpressionAST {
    enum OperationType {
        Plus,
        Minus,
        Multiply,
        Divide
    };

    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    OperationType type;


    explicit BinaryOperationAST(std::unique_ptr<ASTNode> left, OperationType type, std::unique_ptr<ASTNode> right)
            : ExpressionAST(Type::BinaryExpression), left(std::move(left)), right(std::move(right)), type(type) {};
    void accept(ASTVisitor& v) override;
    int64_t execute(pljit_ir::Evaluate& evaluate) override;
};
//---------------------------------------------------------------------------
struct AssignmentAST : public StatementAST {
    std::unique_ptr<IdentifierAST> identifier;
    std::unique_ptr<ASTNode> expression;

    explicit AssignmentAST(std::unique_ptr<IdentifierAST> identifier, std::unique_ptr<ASTNode> expression)
            : StatementAST(Type::AssignmentStatement), identifier(std::move(identifier)),
              expression(std::move(expression)) {};
    void accept(ASTVisitor& v) override;
    int64_t execute(pljit_ir::Evaluate& evaluate) override;
};

//---------------------------------------------------------------------------
struct ReturnStatementAST : StatementAST {
    std::unique_ptr<ASTNode> expression;

    explicit ReturnStatementAST(std::unique_ptr<ASTNode> expression)
            : StatementAST(Type::ReturnStatement), expression(std::move(expression)) {};
    void accept(ASTVisitor& v) override;
    int64_t execute(pljit_ir::Evaluate& evaluate) override;
};
//---------------------------------------------------------------------------
}  // namespace pljit_ast
//---------------------------------------------------------------------------
