#pragma once

#pragma once

#include <vector>
#include <unordered_map>
#include <pljit/parser/PTNode.hpp>
#include "iostream"
#include "Symbol.hpp"
#include "ASTNode.hpp"

namespace pljit_ast {
//---------------------------------------------------------------------------
class SemanticAnalyzer {
    std::unique_ptr<StatementAST> analyzeStatement(pljit_parser::NonTerminalPTNode* node);

    std::unique_ptr<ExpressionAST> analyzeAdditiveExpression(pljit_parser::NonTerminalPTNode* node);

    std::unique_ptr<ExpressionAST> analyzeMultiplicativeExpression(pljit_parser::NonTerminalPTNode* node);

    void analyzeDeclarations(pljit_parser::NonTerminalPTNode* node, Symbol::Type type);

    void analyzeConstDeclarations(pljit_parser::NonTerminalPTNode* node, Symbol::Type type);

public:

    std::unique_ptr<FunctionAST> analyzeParseTree(const std::shared_ptr<pljit_parser::PTNode>& root);
    std::unordered_map<std::string, std::pair<Symbol, unsigned>> symbolTable{};
};
//---------------------------------------------------------------------------
} //namespace pljit_ast