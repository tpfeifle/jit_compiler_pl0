#pragma once

#pragma once

#include <vector>
#include "../source/SourceReference.hpp"
#include "../parser/PTNode.hpp"
#include "iostream"
#include "ASTNode.hpp"
#include "SymbolTable.hpp"


class AST {
    std::unique_ptr<ASTNode> analyzeStatement(std::unique_ptr<PTNode> node);
    std::unique_ptr<ASTNode> analyzeExpression(std::unique_ptr<PTNode> node);
    std::unique_ptr<ASTNode> analyseFunction(std::unique_ptr<PTNode> node);
    std::unique_ptr<ASTNode> analyzeDeclarations(PTNode* node, Symbol::Type type);
    std::unique_ptr<ASTNode> root;
public:
    std::unique_ptr<ASTNode> analyzeParseTree(std::shared_ptr<PTNode> root);
    SymbolTable symbolTable;
    AST() {
        symbolTable = SymbolTable();
    }
};


class DotOutputAST : public VisitorAST {
public:
    void visitPTNode(ASTNode *node) {
    }
};