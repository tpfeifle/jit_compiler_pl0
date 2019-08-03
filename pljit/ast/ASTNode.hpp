#pragma once

#include <vector>

class ASTNode;

class VisitorAST {
public:
    virtual void visitASTNode(ASTNode *node) = 0;
};


class ASTNode {
public:
    /// All possible types of Nodes
    enum class Type {
        Statement,
        Expression,
        Function
    };

    Type type;
    std::vector<ASTNode> children;
    void accept(VisitorAST *v) {
        v->visitASTNode(this);
    }

    explicit ASTNode(const Type type, std::vector<ASTNode> children) : type(type), children(std::move(children)) {}

    Type getType() const; // adding zero means it is a "pure function"

    virtual ~ASTNode() = default; //public virtual destructor
};


struct StatementAST : ASTNode {
    explicit StatementAST(std::vector<ASTNode> children) : ASTNode(ASTNode::Type::Statement, std::move(children)) {};
};
struct ExpressionAST : ASTNode {
    explicit ExpressionAST(std::vector<ASTNode> children) : ASTNode(ASTNode::Type::Expression, std::move(children)) {};
};
struct FunctionAST : ASTNode {
    explicit FunctionAST(std::vector<ASTNode> children) : ASTNode(ASTNode::Type::Function, std::move(children)) {};
};


struct AssignmentExprAST: ExpressionAST {
  //TODO
};

struct ArithmeticExprAST: ExpressionAST {
    //TODO
};