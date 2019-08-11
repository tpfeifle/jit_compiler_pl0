#pragma once
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <pljit/ir/EvalContext.hpp>
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
/// Forward declared visitor for the ASTNode
class ASTVisitor;

//---------------------------------------------------------------------------
/// Node in the abstract syntax tree
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
    /// Constructor
    explicit ASTNode(const Type type) : type(type) {}
    /// Accept method for the visitor pattern
    virtual void accept(ASTVisitor& v) = 0;
    /// Evaluates the node with the provided context
    virtual int64_t execute(pljit_ir::EvalContext& evaluate) = 0;
    /// Get the specific node type
    [[nodiscard]] Type getType() const;
    /// Destructor
    virtual ~ASTNode() = default; //public virtual destructor
private:
    Type type;
};

/// Node representing all kinds of statements in the abstract syntax tree
struct StatementAST : public ASTNode {
    /// Constructor
    explicit StatementAST(Type type) : ASTNode(type) {};
    /// Destructor
    ~StatementAST() override = default;
};

/// Node representing all kinds of expressions in the abstract syntax tree
struct ExpressionAST : ASTNode {
    /// Constructor
    explicit ExpressionAST(const Type type) : ASTNode(type) {};
    /// Destructor
    ~ExpressionAST() override = default;
};

//---------------------------------------------------------------------------
struct FunctionAST : ASTNode {
    /// Children statements of the function
    std::vector<std::unique_ptr<StatementAST>> children;
    /// Constructor
    explicit FunctionAST(std::vector<std::unique_ptr<StatementAST>> children) : ASTNode(ASTNode::Type::Function),
                                                                                children(std::move(children)) {};
    /// Accept for the visitor pattern
    void accept(ASTVisitor& v) override;
    /// Evaluates the node with the provided context
    int64_t execute(pljit_ir::EvalContext& evaluate) override;
};

//---------------------------------------------------------------------------
struct LiteralAST : ExpressionAST {
    /// Constructor
    explicit LiteralAST(int64_t value) : ExpressionAST(ASTNode::Type::LiteralExpression), value(value) {};
    /// Accept for the visitor pattern
    void accept(ASTVisitor& v) override;
    /// Evaluates the node with the provided context
    int64_t execute(pljit_ir::EvalContext& evaluate) override;
    /// Get the value of the literal
    [[nodiscard]] int64_t getValue() const;
private:
    int64_t value;
};

//---------------------------------------------------------------------------
struct UnaryAST : ExpressionAST {
    /// the primary expression of the unary-node
    std::unique_ptr<ASTNode> child;
    enum SignType {
        Plus,
        Minus
    };
    /// Either Plus or Minus
    SignType sign;
    /// Constructor
    explicit UnaryAST(std::unique_ptr<ASTNode> child, SignType sign)
            : ExpressionAST(Type::UnaryExpression), child(std::move(child)), sign(sign) {};
    /// Accept for the visitor pattern
    void accept(ASTVisitor& v) override;
    /// Evaluates the node with the provided context
    int64_t execute(pljit_ir::EvalContext& evaluate) override;
};

//---------------------------------------------------------------------------
struct IdentifierAST : ExpressionAST {
    /// The name of the identifier
    std::string identifier;

    /// Constructor
    explicit IdentifierAST(std::string identifier) : ExpressionAST(Type::IdentifierExpression),
                                                     identifier(std::move(identifier)) {};
    /// Accept for the visitor pattern
    void accept(ASTVisitor& v) override;
    /// Evaluates the node with the provided context
    int64_t execute(pljit_ir::EvalContext& evaluate) override;
};

//---------------------------------------------------------------------------
struct BinaryOperationAST : ExpressionAST {
    enum OperationType {
        Plus,
        Minus,
        Multiply,
        Divide
    };
    /// The left side of the binary expression
    std::unique_ptr<ASTNode> left;
    /// The right side of the binary expression
    std::unique_ptr<ASTNode> right;
    /// The operation used for the expression
    OperationType type;

    /// Constructor
    explicit BinaryOperationAST(std::unique_ptr<ASTNode> left, OperationType type, std::unique_ptr<ASTNode> right)
            : ExpressionAST(Type::BinaryExpression), left(std::move(left)), right(std::move(right)), type(type) {};
    /// Accept for the visitor pattern
    void accept(ASTVisitor& v) override;
    /// Evaluates the node with the provided context
    int64_t execute(pljit_ir::EvalContext& evaluate) override;
};

//---------------------------------------------------------------------------
struct AssignmentAST : public StatementAST {
    /// The identifier that the expression is assigned to
    std::unique_ptr<IdentifierAST> identifier;
    /// The expression inside the statement
    std::unique_ptr<ASTNode> expression;
    /// Constructor
    explicit AssignmentAST(std::unique_ptr<IdentifierAST> identifier, std::unique_ptr<ASTNode> expression)
            : StatementAST(Type::AssignmentStatement), identifier(std::move(identifier)),
              expression(std::move(expression)) {};
    /// Accept for the visitor pattern
    void accept(ASTVisitor& v) override;
    /// Evaluates the node with the provided context
    int64_t execute(pljit_ir::EvalContext& evaluate) override;
};

//---------------------------------------------------------------------------
struct ReturnStatementAST : StatementAST {
    /// The expression inside the return statement
    std::unique_ptr<ASTNode> expression;
    /// Constructor
    explicit ReturnStatementAST(std::unique_ptr<ASTNode> expression)
            : StatementAST(Type::ReturnStatement), expression(std::move(expression)) {};
    /// Accept for the visitor pattern
    void accept(ASTVisitor& v) override;
    /// Evaluates the node with the provided context
    int64_t execute(pljit_ir::EvalContext& evaluate) override;
};
//---------------------------------------------------------------------------
}  // namespace pljit_ast
//---------------------------------------------------------------------------
