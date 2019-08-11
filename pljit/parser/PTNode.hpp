#pragma once
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
/// Visitor for the parse-tree, used to output e.g. the tree in the dot-format
class PTVisitor {
public:
    /// Visitor methods for the visitor pattern
    virtual void visit(LiteralPTNode& node) = 0;
    virtual void visit(IdentifierPTNode& node) = 0;
    virtual void visit(GenericTokenPTNode& node) = 0;
    virtual void visit(NonTerminalPTNode& node) = 0;
};

//---------------------------------------------------------------------------
/// Parse-Tree Node
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
    /// Constructor
    explicit PTNode(const Type type, pljit_source::SourceReference source) : source(source), type(type) {}


    /// Accept method for the visitor pattern
    virtual void accept(PTVisitor& v) = 0;
    /// Virtual Destructor
    virtual ~PTNode() = default;
    /// Get the type of the Parse-Tree node
    [[nodiscard]] Type getType() const;
    /// Reference of the node`s source code
    pljit_source::SourceReference source;
private:
    Type type;
};

//---------------------------------------------------------------------------
struct LiteralPTNode : public PTNode {
    /// Constructor
    explicit LiteralPTNode(pljit_source::SourceReference source, int64_t value) :
            PTNode(PTNode::Type::Literal, source), value(value) {};
    /// Accept method for the visitor pattern
    void accept(PTVisitor& v) override;
    /// Get the value of the literal
    [[nodiscard]] int64_t getValue() const;
private:
    int64_t value;
};

//---------------------------------------------------------------------------
struct IdentifierPTNode : public PTNode {
    /// Constructor
    explicit IdentifierPTNode(pljit_source::SourceReference source) :
            PTNode(PTNode::Type::Identifier, source) {};
    /// Get the specific name of the identifier
    [[nodiscard]] std::string getName() const;
    /// Accept method for the visitor pattern
    void accept(PTVisitor& v) override;
};

//---------------------------------------------------------------------------
struct GenericTokenPTNode : public PTNode {
public:
    /// Constructor
    explicit GenericTokenPTNode(pljit_source::SourceReference source) :
            PTNode(PTNode::Type::GenericToken, source) {};
    /// Accept method for the visitor pattern
    void accept(PTVisitor& v) override;
};

//---------------------------------------------------------------------------
struct OperatorAlternationPTNode : public GenericTokenPTNode {
    enum OperatorType {
        Plus,
        Minus,
        Multiply,
        Divide
    };
    /// Constructor
    explicit OperatorAlternationPTNode(pljit_source::SourceReference source, OperatorType operatorType)
            : GenericTokenPTNode(
            source), operatorType(operatorType) {};
    /// Get the specific operator
    [[nodiscard]] OperatorType getOperatorType() const;
private:
    OperatorType operatorType;
};

//---------------------------------------------------------------------------
struct NonTerminalPTNode : public PTNode {
    /// Constructor
    explicit NonTerminalPTNode(pljit_source::SourceReference source, PTNode::Type type,
                               std::vector<std::unique_ptr<PTNode>> children)
            : PTNode(type, source), children(std::move(children)) {};
    /// Accept method for the visitor pattern
    void accept(PTVisitor& v) override;
    /// Children nodes of this NonTerminal node
    std::vector<std::unique_ptr<PTNode>> children;
};

//---------------------------------------------------------------------------
} //namespace pljit_parser
//---------------------------------------------------------------------------