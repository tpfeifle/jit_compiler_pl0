#pragma once
//---------------------------------------------------------------------------
#include <pljit/source/SourceReference.hpp>
#include <pljit/lexer/Token.hpp>
#include <pljit/lexer/Lexer.hpp>
#include "PTNode.hpp"
#include <vector>
//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
/// Parser transforming the lexer`s token stream into a Parse-Tree
class Parser {
    /// Lexer providing the token stream
    pljit_lexer::Lexer lexer;

    /// Methods to parse the different elements of the token stream
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

    /// Creates a new source-reference containing the range of all the passed children
    pljit_source::SourceReference childrenSourceReference(const std::vector<std::unique_ptr<PTNode>>& children);

    /// The currently by the lexer`s next method provided Token
    std::unique_ptr<pljit_lexer::Token> currentToken;

    /// Flag indicating an error during the parsing of the optional declarations
    bool errorDuringDeclaration = false;
public:
    /// Constructor
    explicit Parser(pljit_lexer::Lexer lexer) : lexer(std::move(lexer)) {}
    /// Public interface to create the parse-tree
    std::unique_ptr<NonTerminalPTNode> parseFunctionDefinition();
};
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------