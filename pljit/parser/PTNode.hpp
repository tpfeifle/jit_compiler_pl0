#pragma once

#include <vector>
#include "../source/SourceReference.hpp"
#include "iostream"
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
class PTNode;

class Visitor {
public:
    virtual void visitPTNode(PTNode* node) = 0;
};


class PTNode {
public:
    /// All possible types of Nodes
    enum class Type {
        Identifier,
        Literal,
        GenericToken,
        BracketLeft, // TODO check if brackets can be genericToken as well (because evtl. operator?)
        BracketRight,
        Assignment,
        FunctionDefinition,
        ParamDeclaration,
        VarDeclaration,
        ConstDeclaration,
        DeclaratorList,
        InitDeclaratorList,
        InitDeclarator,
        CompoundStatement,
        StatementList,
        Statement,
        AssignmentExpr,
        AdditiveExpr,
        MultiplicativeExpr,
        UnaryExpr,
        PrimaryExpr
    };


    SourceReference source;

    void accept(Visitor* v) {
        v->visitPTNode(this);
    }

    explicit PTNode(const Type type, SourceReference source) : source(std::move(source)), type(type) {}

    // Type getType() const; // adding zero means it is a "pure function"

    virtual ~PTNode() = default; //public virtual destructor
    // TODO: Check if the following required for all derived classes
    //      Note that when you delete that pointer, you will leak memory
    //      because you don't have virtual destructors. Just add definitions like this to your classes:

    Type getType() const; // adding zero means it is a "pure function"
private:
    Type type;
};

// TODO: rename into Node instead of Token
class LiteralPTNode : public PTNode {
public:
    explicit LiteralPTNode(SourceReference source, int value) : PTNode(PTNode::Type::Literal, std::move(source)),
                                                                value(value) {};
    int value;
};

class IdentifierPTNode : public PTNode {
public:
    explicit IdentifierPTNode(SourceReference source) : PTNode(PTNode::Type::Identifier, std::move(source)) {};

    std::string getName() {
        return source.getText();
    }
};

class GenericTokenPTNode : public PTNode {
public:
    explicit GenericTokenPTNode(SourceReference source) : PTNode(PTNode::Type::GenericToken, std::move(source)) {};
};

class OperatorAlternationPTNode : public GenericTokenPTNode {
public:
    enum OperatorType {
        Plus,
        Minus,
        Multiply,
        Divide
    };
    OperatorType operatorType;
    explicit OperatorAlternationPTNode(SourceReference source, OperatorType operatorType) : GenericTokenPTNode(
            std::move(source)), operatorType(operatorType) {};
};

struct NonTerminalPTNode : public PTNode {
    std::vector<std::unique_ptr<PTNode>> children;

    explicit NonTerminalPTNode(SourceReference source, PTNode::Type type, std::vector<std::unique_ptr<PTNode>> children)
            :
            PTNode(type, std::move(source)), children(std::move(children)) {};
};

struct NonTerminalAlternationPTNode : public NonTerminalPTNode {
    explicit NonTerminalAlternationPTNode(SourceReference source, PTNode::Type type,
                                          std::vector<std::unique_ptr<PTNode>> children, PTNode::Type chosenAlternation)
            : NonTerminalPTNode(std::move(source), type, std::move(children)), chosenAlternation(chosenAlternation) {};
    PTNode::Type chosenAlternation;
    // TODO handle optional and repeating children
};

// TODO: decide: should we calculate the childrenCount on request (recursively)?
/*
class UnaryExprNode : PTNode {
public:
    explicit UnaryExprNode(SourceReference& source, std::vector<PTNode>& children, unsigned childrenCount) : PTNode(
            PTNode::Type::UnaryExpr, source), children(children), childrenCount(childrenCount) {};
    std::vector<PTNode> children;
    unsigned childrenCount;
};

class PrimaryExprNode : PTNode {
public:
    explicit PrimaryExprNode(SourceReference& source, std::vector<PTNode>& children) : PTNode(PTNode::Type::PrimaryExpr,
                                                                                            source), children(children),
                                                                                     childrenCount(childrenCount) {};
    std::vector<PTNode> children;
    unsigned childrenCount;
    PTNode::Type ownType; // because of alternation this can be either identifier, literal or "(" additive-expression ")". TODO: maybe do not use PTNOde::Type, because parenthesized expression not explicitly a type there
};

class FunctionDefinitionNode : PTNode {
public:
    explicit FunctionDefinitionNode(SourceReference source, std::vector<PTNode> children) : PTNode(
            PTNode::Type::FunctionDefinition, source), children(children), childrenCount(childrenCount) {};
    std::vector<PTNode> children;
    unsigned childrenCount;
    PTNode::Type ownType; // because of alternation this can be either identifier, literal or "(" additive-expression ")". TODO: maybe do not use PTNOde::Type, because parenthesized expression not explicitly a type there
};*/

//---------------------------------------------------------------------------
} //namespace pljit
//---------------------------------------------------------------------------