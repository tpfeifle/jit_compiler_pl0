#pragma once

#include <vector>
#include "../source/SourceReference.hpp"
#include "iostream"
//---------------------------------------------------------------------------
namespace pljit::parser {
//---------------------------------------------------------------------------
struct LiteralPTNode;
struct IdentifierPTNode;
struct GenericTokenPTNode;
struct OperatorAlternationPTNode;
struct NonTerminalPTNode;
struct NonTerminalAlternationPTNode;

class PTVisitor {
public:
    virtual void visit(LiteralPTNode& node) = 0;
    virtual void visit(IdentifierPTNode& node) = 0;
    virtual void visit(GenericTokenPTNode& node) = 0;
    virtual void visit(NonTerminalPTNode& node) = 0;
};


class PTNode {
public:
    /// All possible types of Nodes
    enum class Type {
        Identifier,
        Literal,
        GenericToken,
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

    virtual void accept(PTVisitor& v) = 0;

    explicit PTNode(const Type type, SourceReference source) : source(std::move(source)), type(type) {}

    virtual ~PTNode() = default; //public virtual destructor
    // TODO: Check if the following required for all derived classes
    //      Note that when you delete that pointer, you will leak memory
    //      because you don't have virtual destructors. Just add definitions like this to your classes:

    Type getType() const; // adding zero means it is a "pure function"
private:
    Type type;
};

struct LiteralPTNode : public PTNode {
    explicit LiteralPTNode(SourceReference source, int value) : PTNode(PTNode::Type::Literal, std::move(source)),
                                                                value(value) {};
    void accept(PTVisitor& v) override  {
        v.visit(*this);
    }
    int getValue() const;
private:
    int value;
};

struct IdentifierPTNode : public PTNode {
    explicit IdentifierPTNode(SourceReference source) : PTNode(PTNode::Type::Identifier, std::move(source)) {};

    std::string getName() {
        return source.getText();
    }
    void accept(PTVisitor& v) override  {
        v.visit(*this);
    }
};

struct GenericTokenPTNode : public PTNode {
public:
    explicit GenericTokenPTNode(SourceReference source) : PTNode(PTNode::Type::GenericToken, std::move(source)) {};
    void accept(PTVisitor& v) override  {
        v.visit(*this);
    }
};

struct OperatorAlternationPTNode : public GenericTokenPTNode {
    enum OperatorType {
        Plus,
        Minus,
        Multiply,
        Divide
    };
    explicit OperatorAlternationPTNode(SourceReference source, OperatorType operatorType) : GenericTokenPTNode(
            std::move(source)), operatorType(operatorType) {};
    [[nodiscard]] OperatorType getOperatorType() const;
private:
    OperatorType operatorType;
};

struct NonTerminalPTNode : public PTNode {

    explicit NonTerminalPTNode(SourceReference source, PTNode::Type type, std::vector<std::unique_ptr<PTNode>> children)
            : PTNode(type, std::move(source)), children(std::move(children)) {};
    void accept(PTVisitor& v) override  {
        v.visit(*this);
    }
    // [[nodiscard]] std::vector<std::unique_ptr<PTNode>> getChildren() const;
    std::vector<std::unique_ptr<PTNode>> children;
};

struct NonTerminalAlternationPTNode : public NonTerminalPTNode {
    explicit NonTerminalAlternationPTNode(SourceReference source, PTNode::Type type,
                                          std::vector<std::unique_ptr<PTNode>> children, PTNode::Type chosenAlternation)
            : NonTerminalPTNode(std::move(source), type, std::move(children)), chosenAlternation(chosenAlternation) {};
    PTNode::Type chosenAlternation;
    // TODO handle optional and repeating children
};

//---------------------------------------------------------------------------
} //namespace pljit::parser
//---------------------------------------------------------------------------