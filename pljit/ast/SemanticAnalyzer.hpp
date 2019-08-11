#pragma once
//---------------------------------------------------------------------------
#include <vector>
#include <unordered_map>
#include <pljit/parser/PTNode.hpp>
#include "iostream"
#include "Symbol.hpp"
#include "ASTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
/// Perfoms the semantic analysis of the parse tree
/// returning the abstract syntax tree
class SemanticAnalyzer {
    /// Analyze a statement parse-tree node
    std::unique_ptr<StatementAST> analyzeStatement(pljit_parser::NonTerminalPTNode* node);

    /// Analyze the additive-expression of the parse-tree
    std::unique_ptr<ExpressionAST> analyzeAdditiveExpression(pljit_parser::NonTerminalPTNode* node);

    /// Analyze the multiplicative-expression of the parse-tree
    std::unique_ptr<ExpressionAST> analyzeMultiplicativeExpression(pljit_parser::NonTerminalPTNode* node);

    /// Analyze the declarations of the parse-tree
    void analyzeDeclarations(pljit_parser::NonTerminalPTNode* node, Symbol::Type type);

    /// Analyze the const declarations of the parse-tree
    void analyzeConstDeclarations(pljit_parser::NonTerminalPTNode* node, Symbol::Type type);

public:
    /// The interface to create the ast from the parse-tree
    std::unique_ptr<FunctionAST> analyzeParseTree(std::unique_ptr<pljit_parser::NonTerminalPTNode> root);

    /// Resulting symbol table of the different declared symbols
    /// with their name and the symbol as well as the index at which they were declared
    std::unordered_map<std::string, std::pair<Symbol, unsigned>> symbolTable{};
};
//---------------------------------------------------------------------------
} //namespace pljit_ast
//---------------------------------------------------------------------------