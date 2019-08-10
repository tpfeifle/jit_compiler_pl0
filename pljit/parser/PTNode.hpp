#pragma once

#include <vector>
#include <memory>
#include <pljit/source/SourceReference.hpp>
#include "iostream"
//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
struct LiteralPTNode;
struct IdentifierPTNode;
struct GenericTokenPTNode;
struct OperatorAlternationPTNode;
struct NonTerminalPTNode;

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


    pljit_source::SourceReference source;

    virtual void accept(PTVisitor& v) = 0;

    explicit PTNode(const Type type, pljit_source::SourceReference source) : source(std::move(source)), type(type) {}

    virtual ~PTNode() = default; //public virtual destructor
    // TODO: Check if the following required for all derived classes
    //      Note that when you delete that pointer, you will leak memory
    //      because you don't have virtual destructors. Just add definitions like this to your classes:

    Type getType() const; // adding zero means it is a "pure function"
private:
    Type type;
};

struct LiteralPTNode : public PTNode {
    explicit LiteralPTNode(pljit_source::SourceReference source, int64_t value) : PTNode(PTNode::Type::Literal, std::move(source)),
                                                                value(value) {};
    void accept(PTVisitor& v) override;
    [[nodiscard]] int64_t getValue() const;
    ~LiteralPTNode() override = default;
private:
    int64_t value;
};

struct IdentifierPTNode : public PTNode {
    explicit IdentifierPTNode(pljit_source::SourceReference source) : PTNode(PTNode::Type::Identifier, std::move(source)) {};

    std::string getName() {
        return source.getText();
    }
    void accept(PTVisitor& v) override;
    ~IdentifierPTNode() override = default;
};

struct GenericTokenPTNode : public PTNode {
public:
    explicit GenericTokenPTNode(pljit_source::SourceReference source) : PTNode(PTNode::Type::GenericToken, std::move(source)) {};
    void accept(PTVisitor& v) override;
    ~GenericTokenPTNode() override = default;
};

struct OperatorAlternationPTNode : public GenericTokenPTNode {
    enum OperatorType {
        Plus,
        Minus,
        Multiply,
        Divide
    };
    explicit OperatorAlternationPTNode(pljit_source::SourceReference source, OperatorType operatorType) : GenericTokenPTNode(
            std::move(source)), operatorType(operatorType) {};
    [[nodiscard]] OperatorType getOperatorType() const;
    ~OperatorAlternationPTNode() override = default;
private:
    OperatorType operatorType;
};

struct NonTerminalPTNode : public PTNode {

    explicit NonTerminalPTNode(pljit_source::SourceReference source, PTNode::Type type, std::vector<std::unique_ptr<PTNode>> children)
            : PTNode(type, std::move(source)), children(std::move(children)) {};
    void accept(PTVisitor& v) override;
    // [[nodiscard]] std::vector<std::unique_ptr<PTNode>> getChildren() const;
    std::vector<std::unique_ptr<PTNode>> children;
    ~NonTerminalPTNode() override = default;
};

//---------------------------------------------------------------------------
} //namespace pljit_parser
//---------------------------------------------------------------------------