#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <pljit/ir/Evaluate.hpp>
#include <unordered_map>
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
namespace ast {
//---------------------------------------------------------------------------
class ASTNode;

class VisitorAST {
public:
    virtual void visitASTNode(ASTNode* node) = 0;
};


class ASTNode {
public:
    /// All possible types of Nodes
    enum class Type {
        Statement,
        Expression,
        Function
    };

    int _id;

    void accept(VisitorAST* v) {
        v->visitASTNode(this);
    }

    explicit ASTNode(const Type type) : type(type) {
        static int id = 0;
        _id = id++;
    }

    virtual int execute(Evaluate& evaluate) = 0;

    Type getType() const; // adding zero means it is a "pure function"

    virtual ~ASTNode() = default; //public virtual destructor
private:
    Type type;
};


struct StatementAST : public ASTNode {
    enum class StatementType {
        Assignment,
        ConstAssignment,
        ConstDecl,
        Return
    };
    StatementType statementType;

    explicit StatementAST(StatementType type) : ASTNode(ASTNode::Type::Statement), statementType(type) {};
};

struct ExpressionAST : ASTNode {
    enum class Type {
        Binary,
        Unary,
        Literal,
        Identifier,
    };

    Type type;

    explicit ExpressionAST(const Type type) : ASTNode(ASTNode::Type::Expression), type(type) {};
};

//---------------------------------------------------------------------------
struct FunctionAST : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> children;

    explicit FunctionAST(std::vector<std::unique_ptr<ASTNode>> children) : ASTNode(ASTNode::Type::Function),
                                                                           children(std::move(children)) {};
    int execute(Evaluate& evaluate);
};

//---------------------------------------------------------------------------
struct LiteralAST : ExpressionAST {
    int value;

    explicit LiteralAST(int value) : ExpressionAST(ExpressionAST::Type::Literal), value(value) {};
    int execute(Evaluate& evaluate);
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
            : ExpressionAST(ExpressionAST::Type::Unary), child(std::move(child)), sign(sign) {};
    int execute(Evaluate& evaluate);
};

//---------------------------------------------------------------------------
struct IdentifierAST : ExpressionAST {
    std::string identifier;

    explicit IdentifierAST(std::string identifier) : ExpressionAST(ExpressionAST::Type::Identifier),
                                                     identifier(std::move(identifier)) {};
    int execute(Evaluate& evaluate);
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
            : ExpressionAST(ExpressionAST::Type::Binary), left(std::move(left)), right(std::move(right)), type(type) {};
    int execute(Evaluate& evaluate);
};
//---------------------------------------------------------------------------
struct ConstDeclAST : StatementAST {
    std::vector<std::unique_ptr<ASTNode>> children;

    explicit ConstDeclAST(std::vector<std::unique_ptr<ASTNode>> children) : StatementAST(StatementType::ConstDecl),
                                                                            children(std::move(children)) {};
    int execute(Evaluate& evaluate) override ;
};

//---------------------------------------------------------------------------
struct AssignmentAST : public StatementAST {
    std::unique_ptr<IdentifierAST> identifier;
    std::unique_ptr<ASTNode> expression;

    explicit AssignmentAST(std::unique_ptr<IdentifierAST> identifier, std::unique_ptr<ASTNode> expression)
            : StatementAST(StatementType::Assignment), identifier(std::move(identifier)),
              expression(std::move(expression)) {};
    int execute(Evaluate& evaluate);
};

//---------------------------------------------------------------------------
struct ConstAssignmentAST : StatementAST {
    std::unique_ptr<IdentifierAST> identifier;
    std::unique_ptr<LiteralAST> value;

    explicit ConstAssignmentAST(std::unique_ptr<IdentifierAST> identifier, std::unique_ptr<LiteralAST> value)
            : StatementAST(StatementType::ConstAssignment), identifier(std::move(identifier)),
              value(std::move(value)) {};
    int execute(Evaluate& evaluate);
};

//---------------------------------------------------------------------------
struct ReturnStatementAST : StatementAST {
    std::unique_ptr<ASTNode> expression;

    explicit ReturnStatementAST(std::unique_ptr<ASTNode> expression)
            : StatementAST(StatementType::Return), expression(std::move(expression)) {};
    int execute(Evaluate& evaluate);
};
//---------------------------------------------------------------------------
} //namespace ast
//---------------------------------------------------------------------------
}  // namespace pljit
//---------------------------------------------------------------------------
