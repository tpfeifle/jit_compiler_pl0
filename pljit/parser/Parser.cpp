#include "Parser.hpp"
#include "../lexer/Token.hpp"
#include <stdlib.h>

unsigned lengthOfChildren(const std::vector<PTNode>& children) {
    unsigned length = 0;
    for (PTNode child: children) {
        length += child.source.length;
    }
    return length;
}

SourceReference Parser::childrenSourceReference(const std::vector<PTNode>& children) {
    unsigned length = lengthOfChildren(children);
    return SourceReference(children[0].source.lineNum, children[0].source.charPos, length, lexer.code);
}

std::unique_ptr<PTNode> Parser::parseFunctionDefinition() {
    std::unique_ptr<PTNode> paramDeclaration = parseParameterDeclaration();
    std::unique_ptr<PTNode> varDeclaration = parseVariableDeclaration();
    std::unique_ptr<PTNode> constDeclaration = parseConstDeclaration();
    std::unique_ptr<PTNode> compoundStatement = parseCompoundStatement();
    if (!compoundStatement) {
        std::cout << "Expected compound statement" << std::endl;
        return nullptr;
    }
    std::unique_ptr<Token> final = lexer.next();
    if (!final || (*final).type != Token::Type::FINAL) {
        std::cout << "Expected END keyword" << std::endl;
        std::exit(-1);
        return nullptr;
    }

    std::vector<PTNode> children;
    if (paramDeclaration) {
        children.emplace_back(*paramDeclaration);
    }
    if (varDeclaration) {
        children.emplace_back(*varDeclaration);
    }
    if (constDeclaration) {
        children.emplace_back(*constDeclaration);
    }
    children.emplace_back(*compoundStatement);
    children.emplace_back(GenericToken((*final).source));

    nodes.emplace_back(NonTerminal(childrenSourceReference(children), PTNode::Type::FunctionDefinition, children));
    root = std::make_unique<PTNode>(nodes.back());

    return std::make_unique<PTNode>(nodes.back());
}

/*
 * ------- DECLARATIONS -------
 */

std::unique_ptr<PTNode> Parser::parseParameterDeclaration() {
    std::unique_ptr<Token> paramKeyword = lexer.next();
    if (!paramKeyword || (*paramKeyword).type != Token::Type::PARAM) {
        std::cout << "Expected PARAM keyword" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        std::cout << "Expected declarator list" << std::endl;
        return nullptr;
    }
    std::unique_ptr<Token> semicolonSeparator = lexer.next();
    if (!semicolonSeparator || (*semicolonSeparator).type != Token::Type::SEMICOLON) {
        std::cout << "Expected separator" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::vector<PTNode> children;
    children.emplace_back(GenericToken((*paramKeyword).source));
    children.emplace_back(*declaratorList);
    children.emplace_back(GenericToken((*semicolonSeparator).source));
    return std::make_unique<PTNode>(
            NonTerminal(childrenSourceReference(children), PTNode::Type::ParamDeclaration, children));
}

std::unique_ptr<PTNode> Parser::parseVariableDeclaration() {
    std::unique_ptr<Token> varKeyword = lexer.next();
    if (!varKeyword || (*varKeyword).type != Token::Type::VAR) {
        std::cout << "Expected VAR keyword" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        std::cout << "Expected declarator list" << std::endl;
        return nullptr;
    }
    std::unique_ptr<Token> semicolonSeparator = lexer.next();
    if (!semicolonSeparator || (*semicolonSeparator).type != Token::Type::SEMICOLON) {
        std::cout << "Expected semicolon" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::vector<PTNode> children;
    children.emplace_back(GenericToken((*varKeyword).source));
    children.emplace_back(*declaratorList);
    children.emplace_back(GenericToken((*semicolonSeparator).source));
    return std::make_unique<PTNode>(
            NonTerminal(childrenSourceReference(children), PTNode::Type::VarDeclaration, children));
}

std::unique_ptr<PTNode> Parser::parseConstDeclaration() {
    std::unique_ptr<Token> constKeyword = lexer.next();
    if (!constKeyword || (*constKeyword).type != Token::Type::CONST) {
        std::cout << "Expected CONST keyword" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::unique_ptr<PTNode> declaratorList = parseInitDeclaratorList();
    if (!declaratorList) {
        std::cout << "Expected declarator list" << std::endl;
        return nullptr;
    }
    std::unique_ptr<Token> semicolonSeparator = lexer.next();
    if (!semicolonSeparator || (*semicolonSeparator).type != Token::Type::SEMICOLON) {
        std::cout << "Expected semicolon" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::vector<PTNode> children;
    children.emplace_back(GenericToken((*constKeyword).source));
    children.emplace_back(*declaratorList);
    children.emplace_back(GenericToken((*semicolonSeparator).source));
    return std::make_unique<PTNode>(
            NonTerminal(childrenSourceReference(children), PTNode::Type::ConstDeclaration, children));
}

/*
 * ------- DECLARATOR-LISTS -------
 */


std::unique_ptr<PTNode> Parser::parseDeclaratorList() {
    std::unique_ptr<Token> firstIdentifier = lexer.next();
    if (!firstIdentifier || (*firstIdentifier).category != Token::Category::IDENTIFIER) {
        (*firstIdentifier).source.printContext("Expected identifier");
        std::exit(-1);
        return nullptr;
    }

    std::vector<PTNode> children;
    children.emplace_back(IdentifierToken((*firstIdentifier).source));
    while ((*lexer.nextLookahead()).type == Token::Type::COLON) { // repeat
        std::unique_ptr<Token> colonSeparator = lexer.next();
        if (!colonSeparator || (*colonSeparator).type != Token::Type::COLON) {
            std::cout << "Expected separator" << std::endl;
            std::exit(-1);
            return nullptr;
        }
        std::unique_ptr<Token> identifier = lexer.next();
        if (!identifier || (*identifier).category != Token::Category::IDENTIFIER) {
            (*identifier).source.printContext("Expected identifier");
            std::exit(-1);
            return nullptr;
        }
        children.emplace_back(GenericToken((*colonSeparator).source));
        children.emplace_back(IdentifierToken((*identifier).source));
    }
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::DeclaratorList,
                                                children));
}


std::unique_ptr<PTNode> Parser::parseInitDeclaratorList() {
    std::unique_ptr<PTNode> initDeclarator = parseInitDeclarator();
    if (!initDeclarator) {
        std::cout << "Expected init declarator" << std::endl;
        return nullptr;
    }

    std::vector<PTNode> children;
    children.emplace_back(*initDeclarator);
    while ((*lexer.nextLookahead()).type == Token::Type::COLON) { // repeat
        std::unique_ptr<Token> colonSeparator = lexer.next();
        if (!colonSeparator || (*colonSeparator).type != Token::Type::COLON) {
            std::cout << "Expected colon" << std::endl;
            std::exit(-1);
            return nullptr;
        }
        std::unique_ptr<PTNode> declarator = parseInitDeclarator();
        if (!declarator) {
            std::cout << "Expected declarator" << std::endl;
            return nullptr;
        }
        children.emplace_back(GenericToken((*colonSeparator).source));
        children.emplace_back(*initDeclarator);
    }
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::InitDeclaratorList,
                                                children));
}


std::unique_ptr<PTNode> Parser::parseInitDeclarator() {
    std::unique_ptr<Token> identifier = lexer.next();
    if (!identifier || (*identifier).category != Token::Category::IDENTIFIER) {
        (*identifier).source.printContext("Expected identifier");
        std::exit(-1);
        return nullptr;
    }

    std::unique_ptr<Token> equal = lexer.next();
    if (!equal || (*equal).type != Token::Type::EQUAL) {
        std::cout << "Expected equal sign" << std::endl;
        std::exit(-1);
        return nullptr;
    }

    std::unique_ptr<Token> literal = lexer.next();
    if (!literal || (*literal).category != Token::Category::LITERAL) {
        std::cout << "Expected literal" << std::endl;
        std::exit(-1);
        return nullptr;
    }

    std::vector<PTNode> children;
    children.emplace_back(IdentifierToken((*identifier).source));
    children.emplace_back(GenericToken((*equal).source));
    children.emplace_back(LiteralToken((*literal).source));
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::InitDeclarator,
                                                children));
}


/*
 * ------- STATEMENTS -------
 */

std::unique_ptr<PTNode> Parser::parseCompoundStatement() {
    std::unique_ptr<Token> beginKeyword = lexer.next();
    if (!beginKeyword || (*beginKeyword).type != Token::Type::BEGIN) {
        std::cout << "Expected BEGIN keyword" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::unique_ptr<PTNode> statementList = parseStatementList();
    if (!statementList) {
        std::cout << "Expected statement list" << std::endl;
        return nullptr;
    }
    std::unique_ptr<Token> endKeyword = lexer.next();
    if (!endKeyword || (*endKeyword).type != Token::Type::END) {
        std::cout << "Expected END keyword" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::vector<PTNode> children;
    children.emplace_back(GenericToken((*beginKeyword).source));
    children.emplace_back(*statementList);
    children.emplace_back(PTNode(PTNode::Type::GenericToken, (*endKeyword).source, std::vector<PTNode>{}));
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::CompoundStatement,
                                                children));
}

std::unique_ptr<PTNode> Parser::parseStatementList() {
    std::unique_ptr<PTNode> statement = parseStatement();
    if (!statement) {
        std::cout << "Expected statement" << std::endl;
        return nullptr;
    }
    std::vector<PTNode> children;
    children.emplace_back(*statement);
    while ((*lexer.nextLookahead()).type == Token::Type::SEMICOLON) { // repeat
        std::unique_ptr<Token> semicolonSeparator = lexer.next();
        if (!semicolonSeparator || (*semicolonSeparator).type != Token::Type::SEMICOLON) {
            std::cout << "Expected semicolon" << std::endl;
            std::exit(-1);
            return nullptr;
        }
        std::unique_ptr<PTNode> statementRepeat = parseStatement();
        if (!statementRepeat) {
            std::cout << "Expected statement" << std::endl;
            return nullptr;
        }
        children.emplace_back(GenericToken((*semicolonSeparator).source));
        children.emplace_back(*statementRepeat);
    }
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::StatementList,
                                                children));
}

std::unique_ptr<PTNode> Parser::parseStatement() {
    std::vector<PTNode> children;
    if ((*lexer.nextLookahead()).type == Token::Type::RETURN) {
        std::unique_ptr<Token> returnKeyword = lexer.next();
        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            std::cout << "Expected additive expression" << std::endl;
        return nullptr;
        }
        children.emplace_back(GenericToken((*returnKeyword).source));
        children.emplace_back(*additiveExpr);
    } else {
        std::unique_ptr<PTNode> assignmentExpr = parseAssignmentExpr();
        if (!assignmentExpr) {
            std::cout << "Expected assignment expression" << std::endl;
        return nullptr;
        }
        children.emplace_back(*assignmentExpr);
    }
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::Statement,
                                                children));
}


/*
 * -------- Expressions --------
 */
std::unique_ptr<PTNode> Parser::parseAssignmentExpr() {
    std::unique_ptr<Token> identifier = lexer.next();
    if (!identifier || (*identifier).category != Token::Category::IDENTIFIER) {
        (*identifier).source.printContext("Expected identifier");
        std::exit(-1);
        return nullptr;
    }
    std::unique_ptr<Token> assignment = lexer.next();
    if (!assignment || (*assignment).type != Token::Type::ASSIGNMENT) {
        std::cout << "Expected assignment" << std::endl;
        std::exit(-1);
        return nullptr;
    }
    std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
    if (!additiveExpr) {
        std::cout << "Expected additive expression" << std::endl;
        return nullptr;
    }

    std::vector<PTNode> children;
    children.emplace_back(IdentifierToken((*identifier).source));
    children.emplace_back(GenericToken((*assignment).source));
    children.emplace_back(*additiveExpr);
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::AssignmentExpr,
                                                children));
}

std::unique_ptr<PTNode> Parser::parseAdditiveExpr() {
    std::unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
    if (!multiplicativeExpr) {
        std::cout << "Expected multiplicative expression" << std::endl;
        return nullptr;
    }
    std::vector<PTNode> children;
    children.emplace_back(*multiplicativeExpr);
    if ((*lexer.nextLookahead()).type == Token::Type::PLUS ||
        (*lexer.nextLookahead()).type == Token::Type::MINUS) { // optional
        std::unique_ptr<Token> sign = lexer.next();
        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            std::cout << "Expected additive expression" << std::endl;
        return nullptr;
        }
        children.emplace_back(GenericToken((*sign).source));
        children.emplace_back(*additiveExpr);
    }
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::AdditiveExpr,
                                                children));
}

std::unique_ptr<PTNode> Parser::parseMultiplicativeExpr() {
    std::unique_ptr<PTNode> unaryExpr = parseUnaryExpr();
    if (!unaryExpr) {
        std::cout << "Expected unary expression" << std::endl;
        return nullptr;
    }
    std::vector<PTNode> children;
    children.emplace_back(*unaryExpr);
    if ((*lexer.nextLookahead()).type == Token::Type::MULTIPLY ||
        (*lexer.nextLookahead()).type == Token::Type::DIVIDE) { // optional
        std::unique_ptr<Token> mulDiv = lexer.next();
        std::unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
        if (!multiplicativeExpr) {
            std::cout << "Expected multiplicative expression" << std::endl;
        return nullptr;
        }
        children.emplace_back(GenericToken((*mulDiv).source));
        children.emplace_back(*multiplicativeExpr);
    }
    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::MultiplicativeExpr,
                                                children));
}


std::unique_ptr<PTNode> Parser::parseUnaryExpr() {
    std::vector<PTNode> children;
    if ((*lexer.nextLookahead()).type == Token::Type::PLUS ||
        (*lexer.nextLookahead()).type == Token::Type::MINUS) { // optional
        std::unique_ptr<Token> sign = lexer.next();
        children.emplace_back(GenericToken((*sign).source));
    }

    std::unique_ptr<PTNode> primaryExpr = parsePrimaryExpr();
    if (!primaryExpr) {
        std::cout << "Expected primary epxression" << std::endl;
        return nullptr;
    }
    children.emplace_back(*primaryExpr);


    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::UnaryExpr,
                                                children));
}

std::unique_ptr<PTNode> Parser::parsePrimaryExpr() {
    std::unique_ptr<Token> nextToken = lexer.next();
    if (!nextToken) {
        std::cout << "Expected a next token" << std::endl;
        return nullptr;
    }

    std::vector<PTNode> children;
    if ((*nextToken).category != Token::Category::IDENTIFIER) {
        children.emplace_back(IdentifierToken((*nextToken).source));
    } else if ((*nextToken).category != Token::Category::LITERAL) {
        children.emplace_back(LiteralToken((*nextToken).source));
    } else if ((*nextToken).type != Token::Type::LEFT_BRACKET) {
        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            std::cout << "Expected an additive expression" << std::endl;
            return nullptr;
        }
        std::unique_ptr<Token> rightBracket = lexer.next();
        if (!rightBracket || (*rightBracket).type != Token::Type::RIGHT_BRACKET) {
            std::cout << "Expected a right bracket" << std::endl;
            std::exit(-1);
            return nullptr;
        }
        children.emplace_back(*additiveExpr);
        children.emplace_back(GenericToken((*rightBracket).source));
    }

    return std::make_unique<PTNode>(NonTerminal(childrenSourceReference(children),
                                                PTNode::Type::PrimaryExpr,
                                                children));
}