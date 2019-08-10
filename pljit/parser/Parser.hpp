#pragma once

#include "../lexer/Token.hpp"
#include "../lexer/Lexer.hpp"
#include "../source/SourceReference.hpp"
#include "PTNode.hpp"
#include <vector>
//---------------------------------------------------------------------------
namespace pljit::parser {
//---------------------------------------------------------------------------
class Parser {
    lexer::Lexer lexer;
    std::unique_ptr<PTNode> parseParameterDeclaration();
    std::unique_ptr<PTNode> parseVariableDeclaration();
    std::unique_ptr<PTNode> parseConstDeclaration();
    std::unique_ptr<PTNode> parseDeclaratorList();
    std::unique_ptr<PTNode> parseInitDeclaratorList();
    std::unique_ptr<PTNode> parseInitDeclarator();
    std::unique_ptr<PTNode> parseCompoundStatement();
    std::unique_ptr<PTNode> parseStatementList();
    std::unique_ptr<PTNode> parseStatement();
    std::unique_ptr<PTNode> parseAssignmentExpr();
    std::unique_ptr<PTNode> parseAdditiveExpr();
    std::unique_ptr<PTNode> parseMultiplicativeExpr();
    std::unique_ptr<PTNode> parseUnaryExpr();
    std::unique_ptr<PTNode> parsePrimaryExpr();
    SourceReference childrenSourceReference(const std::vector<std::unique_ptr<PTNode>>& children);

    std::unique_ptr<lexer::Token> currentToken;
    // std::unique_ptr<PTNode> ptRoot; // TODO check if other data structure better suited
public:
    explicit Parser(lexer::Lexer& lexer) : lexer(lexer) {}

    std::unique_ptr<NonTerminalPTNode> parseFunctionDefinition();
};
//---------------------------------------------------------------------------
} // namespace pljit::parser
//---------------------------------------------------------------------------