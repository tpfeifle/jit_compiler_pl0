#pragma once

#include "../lexer/Token.hpp"
#include "../lexer/Lexer.hpp"
#include "../source/SourceReference.hpp"
#include "PTNode.hpp"
#include <vector>

class Parser {
    Lexer lexer;
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
    SourceReference childrenSourceReference(const std::vector<PTNode>& children);

    std::unique_ptr<PTNode> root;
    std::vector<PTNode> nodes; // TODO check if other data structure better suited
public:
    explicit Parser(Lexer& lexer) : lexer(lexer) {}

    std::unique_ptr<PTNode> parseFunctionDefinition();
};
