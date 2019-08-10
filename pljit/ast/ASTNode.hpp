#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <pljit/ir/Evaluate.hpp>
#include <unordered_map>
//---------------------------------------------------------------------------
namespace pljit::ast {
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
    //virtual void visit(StatementAST& node) = 0;
    virtual void visit(AssignmentAST& node) = 0;
    virtual void visit(ReturnStatementAST& node) = 0;
    //virtual void visit(ExpressionAST& node) = 0;
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
        //Statement,
        //Expression,
        //Function
    };

    //TODO int _id;

    virtual void accept(ASTVisitor& v) = 0;

    explicit ASTNode(const Type type) : type(type) {
        // TODO static int id = 0;
        // TODO _id = id++;
    }

    virtual int execute(Evaluate& evaluate) = 0;

    Type getType() const; // adding zero means it is a "pure function"

    virtual ~ASTNode() = default; //public virtual destructor
private:
    Type type;
};


struct StatementAST : public ASTNode {
    /*enum class StatementType {
        Assignment,
        Return
    };
    StatementType statementType;*/

    /*void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }*/

    explicit StatementAST(Type type) : ASTNode(type) {};
};

struct ExpressionAST : ASTNode {
    /*enum class Type {
        Binary,
        Unary,
        Literal,
        Identifier,
    };

    Type type;*/

    /*void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }*/

    explicit ExpressionAST(const Type type) : ASTNode(type) {};
};

//---------------------------------------------------------------------------
struct FunctionAST : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> children;

    explicit FunctionAST(std::vector<std::unique_ptr<ASTNode>> children) : ASTNode(ASTNode::Type::Function),
                                                                           children(std::move(children)) {};

    void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }
    int execute(Evaluate& evaluate) override;
};

//---------------------------------------------------------------------------
struct LiteralAST : ExpressionAST { // TODO: Idea use "LiteralAST : ASTNode" and then the types of ASTNode contain all possible types directly
    int value;

    explicit LiteralAST(int value) : ExpressionAST(Type::LiteralExpression), value(value) {};
    void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }
    int execute(Evaluate& evaluate) override;
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
    void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }
    int execute(Evaluate& evaluate) override;
};

//---------------------------------------------------------------------------
struct IdentifierAST : ExpressionAST {
    std::string identifier;

    explicit IdentifierAST(std::string identifier) : ExpressionAST(Type::IdentifierExpression),
                                                     identifier(std::move(identifier)) {};
    void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }
    int execute(Evaluate& evaluate) override;
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
    void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }
    int execute(Evaluate& evaluate) override;
};
//---------------------------------------------------------------------------
struct AssignmentAST : public StatementAST {
    std::unique_ptr<IdentifierAST> identifier;
    std::unique_ptr<ASTNode> expression;

    explicit AssignmentAST(std::unique_ptr<IdentifierAST> identifier, std::unique_ptr<ASTNode> expression)
            : StatementAST(Type::AssignmentStatement), identifier(std::move(identifier)),
              expression(std::move(expression)) {};
    void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }
    int execute(Evaluate& evaluate) override;
};

//---------------------------------------------------------------------------
struct ReturnStatementAST : StatementAST {
    std::unique_ptr<ASTNode> expression;

    explicit ReturnStatementAST(std::unique_ptr<ASTNode> expression)
            : StatementAST(Type::ReturnStatement), expression(std::move(expression)) {};
    void accept(ASTVisitor& v) override  {
        v.visit(*this);
    }
    int execute(Evaluate& evaluate) override;
};
//---------------------------------------------------------------------------
}  // namespace pljit::ast
//---------------------------------------------------------------------------
