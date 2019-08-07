#include "Parser.hpp"
#include "../lexer/Token.hpp"
#include <stdlib.h>

//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
unsigned lengthOfChildren(const vector<unique_ptr<PTNode>>& children) {
    unsigned length = 0;
    for (const unique_ptr<PTNode>& child: children) {
        length += child->source.length;
    }
    return length;
}
//---------------------------------------------------------------------------
SourceReference Parser::childrenSourceReference(const vector<unique_ptr<PTNode>>& children) {
    unsigned length = lengthOfChildren(children);
    return SourceReference(children[0]->source.lineNum, children[0]->source.charPos, length, lexer.code);
}
//---------------------------------------------------------------------------
unique_ptr<NonTerminalPTNode> Parser::parseFunctionDefinition() {
    unique_ptr<PTNode> paramDeclaration = parseParameterDeclaration();
    unique_ptr<PTNode> varDeclaration = parseVariableDeclaration();
    unique_ptr<PTNode> constDeclaration = parseConstDeclaration();
    unique_ptr<PTNode> compoundStatement = parseCompoundStatement();
    if (!compoundStatement) {
        //cout << "Expected compound statement" << endl;
        return nullptr;
    }
    unique_ptr<Token> final = lexer.next();
    if (!final || final->getType() != Token::Type::FINAL) {
        cout << "Expected END keyword" << endl;
        return nullptr;
    }

    vector<unique_ptr<PTNode>> children;
    if (paramDeclaration) {
        children.emplace_back(move(paramDeclaration));
    }
    if (varDeclaration) {
        children.emplace_back(move(varDeclaration));
    }
    if (constDeclaration) {
        children.emplace_back(move(constDeclaration));
    }
    children.emplace_back(move(compoundStatement));
    children.emplace_back(make_unique<GenericTokenPTNode>(final->source));

    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::FunctionDefinition,
                                          move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseParameterDeclaration() {
    unique_ptr<Token> paramKeyword = lexer.next();
    if (!paramKeyword || paramKeyword->getType() != Token::Type::PARAM) {
        cout << "Expected PARAM keyword" << endl;
        return nullptr;
    }
    unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        cout << "Expected declarator list" << endl;
        return nullptr;
    }
    unique_ptr<Token> semicolonSeparator = lexer.next();
    if (!semicolonSeparator || semicolonSeparator->getType() != Token::Type::SEMICOLON) {
        cout << "Expected separator" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<GenericTokenPTNode>(paramKeyword->source));
    children.emplace_back(move(declaratorList));
    children.emplace_back(make_unique<GenericTokenPTNode>(semicolonSeparator->source));
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::ParamDeclaration,
                                          move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseVariableDeclaration() {
    unique_ptr<Token> varKeyword = lexer.next();
    if (!varKeyword || (*varKeyword).getType() != Token::Type::VAR) {
        cout << "Expected VAR keyword" << endl;
        return nullptr;
    }
    unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        cout << "Expected declarator list" << endl;
        return nullptr;
    }
    unique_ptr<Token> semicolonSeparator = lexer.next();
    if (!semicolonSeparator || semicolonSeparator->getType() != Token::Type::SEMICOLON) {
        cout << "Expected semicolon" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<GenericTokenPTNode>(varKeyword->source));
    children.emplace_back(move(declaratorList));
    children.emplace_back(make_unique<GenericTokenPTNode>(semicolonSeparator->source));
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::VarDeclaration,
                                          move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseConstDeclaration() {
    unique_ptr<Token> constKeyword = lexer.next();
    if (!constKeyword || constKeyword->getType() != Token::Type::CONST) {
        cout << "Expected CONST keyword" << endl;
        return nullptr;
    }
    unique_ptr<PTNode> declaratorList = parseInitDeclaratorList();
    if (!declaratorList) {
        cout << "Expected declarator list" << endl;
        return nullptr;
    }
    unique_ptr<Token> semicolonSeparator = lexer.next();
    if (!semicolonSeparator || semicolonSeparator->getType() != Token::Type::SEMICOLON) {
        cout << "Expected semicolon" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<GenericTokenPTNode>(constKeyword->source));
    children.emplace_back(move(declaratorList));
    children.emplace_back(make_unique<GenericTokenPTNode>(semicolonSeparator->source));
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::ConstDeclaration,
                                          move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseDeclaratorList() {
    unique_ptr<Token> firstIdentifier = lexer.next();
    if (!firstIdentifier || firstIdentifier->category != Token::Category::IDENTIFIER) {
        firstIdentifier->source.printContext("Expected identifier");
        return nullptr;
    }

    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<IdentifierPTNode>(firstIdentifier->source));
    while ((*lexer.nextLookahead()).getType() == Token::Type::COLON) { // repeat
        unique_ptr<Token> colonSeparator = lexer.next();
        if (!colonSeparator || colonSeparator->getType() != Token::Type::COLON) {
            cout << "Expected separator" << endl;
            return nullptr;
        }
        unique_ptr<Token> identifier = lexer.next();
        if (!identifier || identifier->category != Token::Category::IDENTIFIER) {
            identifier->source.printContext("Expected identifier");
            return nullptr;
        }
        children.emplace_back(make_unique<GenericTokenPTNode>(colonSeparator->source));
        children.emplace_back(make_unique<IdentifierPTNode>(identifier->source));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::DeclaratorList, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseInitDeclaratorList() {
    unique_ptr<PTNode> initDeclarator = parseInitDeclarator();
    if (!initDeclarator) {
        cout << "Expected init declarator" << endl;
        return nullptr;
    }

    vector<unique_ptr<PTNode>> children;
    children.emplace_back(move(initDeclarator));
    while ((*lexer.nextLookahead()).getType() == Token::Type::COLON) { // repeat
        unique_ptr<Token> colonSeparator = lexer.next();
        if (!colonSeparator || colonSeparator->getType() != Token::Type::COLON) {
            cout << "Expected colon" << endl;
            return nullptr;
        }
        unique_ptr<PTNode> declarator = parseInitDeclarator();
        if (!declarator) {
            cout << "Expected declarator" << endl;
            return nullptr;
        }
        children.emplace_back(make_unique<GenericTokenPTNode>(colonSeparator->source));
        children.emplace_back(move(initDeclarator));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::InitDeclaratorList, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseInitDeclarator() {
    unique_ptr<Token> identifier = lexer.next();
    if (!identifier || identifier->category != Token::Category::IDENTIFIER) {
        identifier->source.printContext("Expected identifier");
        return nullptr;
    }

    unique_ptr<Token> equal = lexer.next();
    if (!equal || equal->getType() != Token::Type::EQUAL) {
        cout << "Expected equal sign" << endl;
        return nullptr;
    }

    unique_ptr<Token> literal = lexer.next();
    if (!literal || literal->category != Token::Category::LITERAL) {
        cout << "Expected literal" << endl;
        return nullptr;
    }

    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<IdentifierPTNode>(identifier->source));
    children.emplace_back(make_unique<GenericTokenPTNode>(equal->source));
    children.emplace_back(make_unique<LiteralPTNode>(literal->source, stoi(literal->source.getText(),
                                                                           nullptr, 10)));
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::InitDeclarator, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseCompoundStatement() {
    unique_ptr<Token> beginKeyword = lexer.next();
    if (!beginKeyword || beginKeyword->getType() != Token::Type::BEGIN) {
        cout << "Expected BEGIN keyword" << endl;
        return nullptr;
    }
    unique_ptr<PTNode> statementList = parseStatementList();
    if (!statementList) {
        cout << "Expected statement list" << endl;
        return nullptr;
    }
    unique_ptr<Token> endKeyword = lexer.next();
    if (!endKeyword || endKeyword->getType() != Token::Type::END) {
        cout << "Expected END keyword" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<GenericTokenPTNode>(beginKeyword->source));
    children.emplace_back(move(statementList));
    children.emplace_back(make_unique<GenericTokenPTNode>(endKeyword->source));
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::CompoundStatement, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseStatementList() {
    unique_ptr<PTNode> statement = parseStatement();
    if (!statement) {
        cout << "Expected statement" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(move(statement));
    while ((*lexer.nextLookahead()).getType() == Token::Type::SEMICOLON) { // repeat
        unique_ptr<Token> semicolonSeparator = lexer.next();
        if (!semicolonSeparator || semicolonSeparator->getType() != Token::Type::SEMICOLON) {
            cout << "Expected semicolon" << endl;
            return nullptr;
        }
        unique_ptr<PTNode> statementRepeat = parseStatement();
        if (!statementRepeat) {
            cout << "Expected statement" << endl;
            return nullptr;
        }
        children.emplace_back(make_unique<GenericTokenPTNode>(semicolonSeparator->source));
        children.emplace_back(move(statementRepeat));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::StatementList, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseStatement() {
    vector<unique_ptr<PTNode>> children;
    PTNode::Type alternationType;
    
    if ((*lexer.nextLookahead()).getType() == Token::Type::RETURN) {
        unique_ptr<Token> returnKeyword = lexer.next();
        unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            cout << "Expected additive expression" << endl;
            return nullptr;
        }
        children.emplace_back(make_unique<GenericTokenPTNode>((*returnKeyword).source));
        children.emplace_back(move(additiveExpr));
        alternationType = PTNode::Type::AdditiveExpr;
    } else {
        unique_ptr<PTNode> assignmentExpr = parseAssignmentExpr();
        if (!assignmentExpr) {
            cout << "Expected assignment expression" << endl;
            return nullptr;
        }
        children.emplace_back(move(assignmentExpr));
        alternationType = PTNode::Type::AssignmentExpr;
    }
    return make_unique<NonTerminalAlternationPTNode>(childrenSourceReference(children),
                                                     PTNode::Type::Statement, move(children),
                                                     alternationType);
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseAssignmentExpr() {
    unique_ptr<Token> identifier = lexer.next();
    if (!identifier || identifier->category != Token::Category::IDENTIFIER) {
        identifier->source.printContext("Expected identifier");
        return nullptr;
    }
    unique_ptr<Token> assignment = lexer.next();
    if (!assignment || assignment->getType() != Token::Type::ASSIGNMENT) {
        cout << "Expected assignment" << endl;
        return nullptr;
    }
    unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
    if (!additiveExpr) {
        cout << "Expected additive expression" << endl;
        return nullptr;
    }

    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<IdentifierPTNode>(identifier->source));
    children.emplace_back(make_unique<GenericTokenPTNode>(assignment->source));
    children.emplace_back(move(additiveExpr));
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::AssignmentExpr, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseAdditiveExpr() {
    unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
    if (!multiplicativeExpr) {
        cout << "Expected multiplicative expression" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(move(multiplicativeExpr));
    if ((*lexer.nextLookahead()).getType() == Token::Type::PLUS ||
        (*lexer.nextLookahead()).getType() == Token::Type::MINUS) { // optional
        unique_ptr<Token> sign = lexer.next();
        unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            cout << "Expected additive expression" << endl;
            return nullptr;
        }

        if (sign->getType() == Token::Type::PLUS) {
            children.emplace_back(
                    make_unique<OperatorAlternationPTNode>(sign->source, OperatorAlternationPTNode::Plus));
        } else {
            children.emplace_back(
                    make_unique<OperatorAlternationPTNode>(sign->source, OperatorAlternationPTNode::Minus));
        }

        children.emplace_back(move(additiveExpr));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::AdditiveExpr, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseMultiplicativeExpr() {
    unique_ptr<PTNode> unaryExpr = parseUnaryExpr();
    if (!unaryExpr) {
        cout << "Expected unary expression" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(move(unaryExpr));
    if ((*lexer.nextLookahead()).getType() == Token::Type::MULTIPLY ||
        (*lexer.nextLookahead()).getType() == Token::Type::DIVIDE) { // optional
        unique_ptr<Token> mulDiv = lexer.next();
        unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
        if (!multiplicativeExpr) {
            cout << "Expected multiplicative expression" << endl;
            return nullptr;
        }
        if (mulDiv->getType() == Token::Type::MULTIPLY) {
            children.emplace_back(
                    make_unique<OperatorAlternationPTNode>(mulDiv->source, OperatorAlternationPTNode::Multiply));
        } else {
            children.emplace_back(
                    make_unique<OperatorAlternationPTNode>(mulDiv->source, OperatorAlternationPTNode::Divide));
        }
        children.emplace_back(move(multiplicativeExpr));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::MultiplicativeExpr, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseUnaryExpr() {
    vector<unique_ptr<PTNode>> children;
    if ((*lexer.nextLookahead()).getType() == Token::Type::PLUS ||
        (*lexer.nextLookahead()).getType() == Token::Type::MINUS) { // optional
        unique_ptr<Token> sign = lexer.next();
        if (sign->getType() == Token::Type::PLUS) {
            children.emplace_back(
                    make_unique<OperatorAlternationPTNode>(sign->source, OperatorAlternationPTNode::Plus));
        } else {
            children.emplace_back(
                    make_unique<OperatorAlternationPTNode>(sign->source, OperatorAlternationPTNode::Minus));
        }
        children.emplace_back(make_unique<GenericTokenPTNode>(sign->source));
    }

    unique_ptr<PTNode> primaryExpr = parsePrimaryExpr();
    if (!primaryExpr) {
        cout << "Expected primary epxression" << endl;
        return nullptr;
    }
    children.emplace_back(move(primaryExpr));


    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::UnaryExpr, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parsePrimaryExpr() {
    unique_ptr<Token> nextToken = lexer.next();
    if (!nextToken) {
        cout << "Expected a next token" << endl;
        return nullptr;
    }

    vector<unique_ptr<PTNode>> children;
    PTNode::Type alternationType;
    if (nextToken->category == Token::Category::IDENTIFIER) {
        children.emplace_back(make_unique<IdentifierPTNode>(nextToken->source));
        alternationType = PTNode::Type::Identifier;
    } else if (nextToken->category == Token::Category::LITERAL) {
        children.emplace_back(make_unique<LiteralPTNode>(nextToken->source,
                                                         stoi(nextToken->source.getText(),
                                                              nullptr, 10)));
        alternationType = PTNode::Type::Literal;
    } else if (nextToken->getType() == Token::Type::LEFT_BRACKET) {
        unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            cout << "Expected an additive expression" << endl;
            return nullptr;
        }
        unique_ptr<Token> rightBracket = lexer.next();
        if (!rightBracket || rightBracket->getType() != Token::Type::RIGHT_BRACKET) {
            cout << "Expected a right bracket" << endl;
            return nullptr;
        }
        children.emplace_back(move(additiveExpr));
        children.emplace_back(make_unique<GenericTokenPTNode>(rightBracket->source));
        alternationType = PTNode::Type::AdditiveExpr;
    }

    return make_unique<NonTerminalAlternationPTNode>(childrenSourceReference(children), PTNode::Type::PrimaryExpr,
                                                     move(children), alternationType);
}
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------