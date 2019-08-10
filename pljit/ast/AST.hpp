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
    std::unique_ptr<StatementAST> analyzeStatement(parser::NonTerminalPTNode* node);

    std::unique_ptr<ExpressionAST> analyzeAdditiveExpression(parser::NonTerminalPTNode* node);

    std::unique_ptr<ExpressionAST> analyzeMultiplicativeExpression(parser::NonTerminalPTNode* node);

    void analyzeDeclarations(parser::NonTerminalPTNode* node, Symbol::Type type);

    void analyzeConstDeclarations(parser::NonTerminalPTNode* node, Symbol::Type type);

public:

    std::unique_ptr<FunctionAST> analyzeParseTree(std::shared_ptr<parser::PTNode> root);
    unsigned currentSymbolId = 0;
    std::unordered_map<std::string, std::pair<Symbol, unsigned>> symbolTable{};
};
//---------------------------------------------------------------------------
} //namespace pljit::ast