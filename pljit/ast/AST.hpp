#pragma once

#pragma once

#include <vector>
#include <unordered_map>
#include "../source/SourceReference.hpp"
#include "../parser/PTNode.hpp"
#include "iostream"
#include "ASTNode.hpp"
#include "SymbolTable.hpp"

namespace pljit::ast {
//---------------------------------------------------------------------------
class AST {
    std::unique_ptr<ASTNode> analyzeStatement(NonTerminalPTNode* node);

    std::unique_ptr<ASTNode> analyzeAdditiveExpression(NonTerminalPTNode* node);

    std::unique_ptr<ASTNode> analyzeMultiplicativeExpression(NonTerminalPTNode* node);

    std::unique_ptr<ASTNode> analyzeDeclarations(NonTerminalPTNode* node, Symbol::Type type);

    std::unique_ptr<ASTNode> analyzeConstDeclarations(NonTerminalPTNode* node, Symbol::Type type);

public:

    std::unique_ptr<ASTNode> analyzeParseTree(std::shared_ptr<PTNode> root);

    std::unordered_map<std::string, Symbol> symbolTable{};
};


class DotOutputAST : public VisitorAST {
public:
    void visitPTNode(ASTNode* node);
};
//---------------------------------------------------------------------------
} //namespace pljit::ast