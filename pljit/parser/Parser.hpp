#pragma once

#include <pljit/source/SourceReference.hpp>
#include <pljit/lexer/Token.hpp>
#include <pljit/lexer/Lexer.hpp>
#include "PTNode.hpp"
#include <vector>
//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
class Parser {
    pljit_lexer::Lexer lexer;
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
    pljit_source::SourceReference childrenSourceReference(const std::vector<std::unique_ptr<PTNode>>& children);

    std::unique_ptr<pljit_lexer::Token> currentToken;
public:
    explicit Parser(pljit_lexer::Lexer& lexer) : lexer(lexer) {}
    std::unique_ptr<NonTerminalPTNode> parseFunctionDefinition();
};
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------