#pragma once

#include <vector>
#include "../source/SourceReference.hpp"
#include "iostream"

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

    Type type;
    SourceReference source;
    std::vector<PTNode> children;

    void accept(Visitor* v) {
        v->visitPTNode(this);
    }

    explicit PTNode(const Type type, SourceReference source, std::vector<PTNode> children) : type(type),
                                                                                             source(std::move(source)),
                                                                                             children(std::move(
                                                                                                     children)) {}

    Type getType() const; // adding zero means it is a "pure function"

    virtual ~PTNode() = default; //public virtual destructor
    // TODO: Check if the following required for all derived classes
    //      Note that when you delete that pointer, you will leak memory
    //      because you don't have virtual destructors. Just add definitions like this to your classes:
};

// TODO: rename into Node instead of Token
class LiteralToken : public PTNode {
public:
    explicit LiteralToken(SourceReference source) : PTNode(PTNode::Type::Literal,
                                                                                         std::move(source),
                                                                                         std::vector<PTNode>{}) {};
    int value;
};

class IdentifierToken : public PTNode {
public:
    explicit IdentifierToken(SourceReference source) : PTNode(PTNode::Type::Identifier,
                                                                                            std::move(source),
                                                                                            std::vector<PTNode>{}) {};
    std::string getName() {
        return source.getText();
    }
};

class GenericToken : public PTNode {
public:
    explicit GenericToken(SourceReference source) : PTNode(PTNode::Type::GenericToken, std::move(source),
                                                           std::vector<PTNode>{}) {};
};

struct NonTerminal : public PTNode {
    explicit NonTerminal(SourceReference source, PTNode::Type type, std::vector<PTNode> children) :
            PTNode(type, std::move(source),
                   std::move(
                           children)) {};
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




class DotOutput : public Visitor {
    std::string nodeTypeToString(PTNode::Type type) {
        std::string label;
        switch (type) {
            case PTNode::Type::Statement:
                label = "Statement";
                break;
            case PTNode::Type::Identifier:
                label = "Identifier";
                break;
            case PTNode::Type::Literal:
                label = "Literal";
                break;
            case PTNode::Type::GenericToken:
                label = "GenericToken";
                break;
            case PTNode::Type::AdditiveExpr:
                label = "AdditiveExpr";
                break;
            case PTNode::Type::MultiplicativeExpr:
                label = "MultiplicativeExpr";
                break;
            case PTNode::Type::AssignmentExpr:
                label = "AssignmentExpr";
                break;
            case PTNode::Type::UnaryExpr:
                label = "UnaryExpr";
                break;
            case PTNode::Type::PrimaryExpr:
                label = "PrimaryExpr";
                break;
            case PTNode::Type::StatementList:
                label = "StatementList";
                break;
            case PTNode::Type::CompoundStatement:
                label = "CompoundStatement";
                break;
            case PTNode::Type::InitDeclarator:
                label = "InitDeclarator";
                break;
            case PTNode::Type::InitDeclaratorList:
                label = "InitDeclaratorList";
                break;
            case PTNode::Type::DeclaratorList:
                label = "DeclaratorList";
                break;
            case PTNode::Type::ConstDeclaration:
                label = "ConstDeclaration";
                break;
            case PTNode::Type::VarDeclaration:
                label = "VarDeclaration";
                break;
            case PTNode::Type::ParamDeclaration:
                label = "ParamDeclaration";
                break;
            default:
                label = "FunctionDefinition";
                break;
        }
        return label;
    }
public:
    void visitPTNode(PTNode* node) {
        std::string label = nodeTypeToString(node->type);

        std::cout << label << node->source.toString() << "[label=\"" << label << "\"];" << std::endl;
        for (PTNode child: node->children) {
            std::cout << label << node->source.toString() << " -> " << nodeTypeToString(child.type) << child.source.toString() << std::endl;
            visitPTNode(&child);
        }
    }
};