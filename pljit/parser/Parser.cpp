#include "Parser.hpp"
#include "../lexer/Token.hpp"
#include <stdlib.h>
#include <inttypes.h>

//---------------------------------------------------------------------------
using namespace std;
using namespace pljit::lexer;
//---------------------------------------------------------------------------
namespace pljit::parser {
//---------------------------------------------------------------------------
unsigned lengthOfChildren(const vector<unique_ptr<PTNode>>& children) {
    unsigned length = 0;
    for (const unique_ptr<PTNode>& child: children) {
        length += child->source.length;
    }
    return length;
}
//---------------------------------------------------------------------------
source::SourceReference Parser::childrenSourceReference(const vector<unique_ptr<PTNode>>& children) {
    unsigned length = lengthOfChildren(children);
    return source::SourceReference(children[0]->source.lineNum, children[0]->source.charPos, length, lexer.code);
}
//---------------------------------------------------------------------------
unique_ptr<NonTerminalPTNode> Parser::parseFunctionDefinition() {
    currentToken = lexer.next();
    unique_ptr<PTNode> paramDeclaration = parseParameterDeclaration();
    unique_ptr<PTNode> varDeclaration = parseVariableDeclaration();
    unique_ptr<PTNode> constDeclaration = parseConstDeclaration();
    unique_ptr<PTNode> compoundStatement = parseCompoundStatement();
    if (!compoundStatement) {
        //cout << "Expected compound statement" << endl;
        return nullptr;
    }
    if (!currentToken || currentToken->getType() != Token::Type::Final) {
        cout << "Expected Final token" << endl;
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
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // final token "."

    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::FunctionDefinition,
                                          move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseParameterDeclaration() {
    if (currentToken->getType() != Token::Type::Param) {
        // cout << "Expected Param keyword" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // param keyword

    currentToken = lexer.next();
    unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        // currentToken->source.printContext("Expected declarator list");
        return nullptr;
    }
    if (!currentToken || currentToken->getType() != Token::Type::Semicolon) {
        currentToken->source.printContext("Expected separator");
        return nullptr;
    }

    children.emplace_back(move(declaratorList));
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator

    currentToken = lexer.next();
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::ParamDeclaration,
                                          move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseVariableDeclaration() {
    vector<unique_ptr<PTNode>> children;
    if (!currentToken || currentToken->getType() != Token::Type::Var) {
        // cout << "Expected Var keyword" << endl;
        return nullptr;
    }
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // var keyword
    currentToken = lexer.next();

    unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        // currentToken->source.printContext("Expected declarator list");
        return nullptr;
    }

    if (!currentToken || currentToken->getType() != Token::Type::Semicolon) {
        currentToken->source.printContext("Expected semicolon");
        return nullptr;
    }
    children.emplace_back(move(declaratorList));
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator

    currentToken = lexer.next();
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::VarDeclaration,
                                          move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseConstDeclaration() {
    if (!currentToken || currentToken->getType() != Token::Type::Const) {
        // cout << "Expected Const keyword" << endl;
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // const keyword
    currentToken = lexer.next();

    unique_ptr<PTNode> declaratorList = parseInitDeclaratorList();
    if (!declaratorList) {
        // currentToken->source.printContext("Expected declarator list");
        return nullptr;
    }

    if (!currentToken || currentToken->getType() != Token::Type::Semicolon) {
        currentToken->source.printContext("Expected semicolon");
        return nullptr;
    }
    children.emplace_back(move(declaratorList));
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator
    currentToken = lexer.next();
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::ConstDeclaration,
                                          move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseDeclaratorList() {
    if (currentToken->getType() != Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<IdentifierPTNode>(currentToken->source));

    currentToken = lexer.next();
    while ((currentToken->getType() == Token::Type::Colon)) {
        children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source));

        currentToken = lexer.next();
        if (currentToken->getType() != Token::Type::Identifier) {
            currentToken->source.printContext("Expected identifier");
            return nullptr;
        }
        children.emplace_back(make_unique<IdentifierPTNode>(currentToken->source));
        currentToken = lexer.next();
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::DeclaratorList, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseInitDeclaratorList() {
    unique_ptr<PTNode> initDeclarator = parseInitDeclarator();
    if (!initDeclarator) {
        // currentToken->source.printContext("Expected init declarator");
        return nullptr;
    }

    vector<unique_ptr<PTNode>> children;
    children.emplace_back(move(initDeclarator));

    while (currentToken->getType() == Token::Type::Colon) { // repeat
        if (!currentToken || currentToken->getType() != Token::Type::Colon) {
            currentToken->source.printContext("Expected colon");
            return nullptr;
        }
        children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // colon separator
        currentToken = lexer.next();
        unique_ptr<PTNode> declarator = parseInitDeclarator();
        if (!declarator) {
            // currentToken->source.printContext("Expected declarator");
            return nullptr;
        }

        children.emplace_back(move(declarator));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::InitDeclaratorList, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseInitDeclarator() {
    if (!currentToken || currentToken->getType() != Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<IdentifierPTNode>(currentToken->source)); // identifier
    currentToken = lexer.next();

    if (!currentToken || currentToken->getType() != Token::Type::Equal) {
        currentToken->source.printContext("Expected equal sign");
        return nullptr;
    }
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // equal sign

    currentToken = lexer.next();
    if (!currentToken || currentToken->getType() != Token::Type::Literal) {
        currentToken->source.printContext("Expected literal");
        return nullptr;
    }
    children.emplace_back(make_unique<LiteralPTNode>(currentToken->source, stoll(currentToken->source.getText(), nullptr, 10)));
    currentToken = lexer.next();
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::InitDeclarator, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseCompoundStatement() {
    if (!currentToken || currentToken->getType() != Token::Type::Begin) {
        currentToken->source.printContext("Expected BEGIN keyword");
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // beginKeyword
    currentToken = lexer.next();

    unique_ptr<PTNode> statementList = parseStatementList();
    if (!statementList) {
        // currentToken->source.printContext("Expected statement list");
        return nullptr;
    }
    if (!currentToken || currentToken->getType() != Token::Type::End) {
        currentToken->source.printContext("Expected END keyword");
        return nullptr;
    }

    children.emplace_back(move(statementList));
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // end keyword
    currentToken = lexer.next();
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::CompoundStatement, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseStatementList() {
    unique_ptr<PTNode> statement = parseStatement();
    if (!statement) {
        // currentToken->source.printContext("Expected statement");
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(move(statement));

    while (currentToken->getType() == Token::Type::Semicolon) { // repeat
        children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator
        currentToken = lexer.next();

        unique_ptr<PTNode> statementRepeat = parseStatement();
        if (!statementRepeat) {
            // currentToken->source.printContext("Expected statement");
            return nullptr;
        }
        children.emplace_back(move(statementRepeat));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::StatementList, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseStatement() {
    vector<unique_ptr<PTNode>> children;

    if (currentToken && currentToken->getType() == Token::Type::Return) {
        children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source));
        currentToken = lexer.next();
        unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            // currentToken->source.printContext("Expected additive expression");
            return nullptr;
        }

        children.emplace_back(move(additiveExpr));
    } else {
        unique_ptr<PTNode> assignmentExpr = parseAssignmentExpr();
        if (!assignmentExpr) {
            // currentToken->source.printContext("Expected assignment expression");
            return nullptr;
        }
        children.emplace_back(move(assignmentExpr));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                                     PTNode::Type::Statement, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseAssignmentExpr() {
    if (!currentToken || currentToken->getType() != Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(make_unique<IdentifierPTNode>(currentToken->source)); // identifier

    currentToken = lexer.next();
    if (!currentToken || currentToken->getType() != Token::Type::Assignment) {
        currentToken->source.printContext("Expected assignment");
        return nullptr;
    }
    children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // assignment
    currentToken = lexer.next();

    unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
    if (!additiveExpr) {
        // currentToken->source.printContext("Expected additive expression");
        return nullptr;
    }

    children.emplace_back(move(additiveExpr));
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::AssignmentExpr, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseAdditiveExpr() {
    unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
    if (!multiplicativeExpr) {
        // currentToken->source.printContext("Expected multiplicative expression");
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(move(multiplicativeExpr));

    if(currentToken->getType() == Token::Type::Plus) {
        children.emplace_back(
                make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Plus));
        currentToken = lexer.next();
        unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            // currentToken->source.printContext("Expected additive expression");
            return nullptr;
        }
        children.emplace_back(move(additiveExpr));
    } else if(currentToken->getType() == Token::Type::Minus) {
        children.emplace_back(
                make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Minus));
        currentToken = lexer.next();
        unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            // currentToken->source.printContext("Expected additive expression");
            return nullptr;
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
        // currentToken->source.printContext("Expected unary expression");
        return nullptr;
    }
    vector<unique_ptr<PTNode>> children;
    children.emplace_back(move(unaryExpr));
    if(currentToken->getType() == Token::Type::Multiply) { // optional
        children.emplace_back(
                make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Multiply));
        currentToken = lexer.next();
        unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
        if (!multiplicativeExpr) {
            // currentToken->source.printContext("Expected multiplicative expression");
            return nullptr;
        }
        children.emplace_back(move(multiplicativeExpr));
    } else if (currentToken->getType() == Token::Type::Divide) { // optional
        children.emplace_back(
                make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Divide));
        currentToken = lexer.next();
        unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
        if (!multiplicativeExpr) {
            // currentToken->source.printContext("Expected multiplicative expression");
            return nullptr;
        }
        children.emplace_back(move(multiplicativeExpr));
    }
    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::MultiplicativeExpr, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parseUnaryExpr() {
    vector<unique_ptr<PTNode>> children;
    if (currentToken->getType() == Token::Type::Plus) { // optional
        children.emplace_back(
                make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Plus));
        children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source));
        currentToken = lexer.next();
    } else if(currentToken->getType() == Token::Type::Minus) { // optional
        children.emplace_back(
                make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Minus));
        children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source));
        currentToken = lexer.next();
    }

    unique_ptr<PTNode> primaryExpr = parsePrimaryExpr();
    if (!primaryExpr) {
        // currentToken->source.printContext("Expected primary expression");
        return nullptr;
    }
    children.emplace_back(move(primaryExpr));


    return make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                          PTNode::Type::UnaryExpr, move(children));
}
//---------------------------------------------------------------------------
unique_ptr<PTNode> Parser::parsePrimaryExpr() {
    if (!currentToken) {
        currentToken->source.printContext("Expected a next token");
        return nullptr;
    }

    vector<unique_ptr<PTNode>> children;
    if (currentToken->getType() == Token::Type::Identifier) {
        children.emplace_back(make_unique<IdentifierPTNode>(currentToken->source));
    } else if (currentToken->getType() == Token::Type::Literal) {
        children.emplace_back(make_unique<LiteralPTNode>(currentToken->source,
                                                         stoi(currentToken->source.getText(),
                                                              nullptr, 10)));
    } else if (currentToken->getType() == Token::Type::Left_Bracket) {
        children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); // left bracket
        currentToken = lexer.next();

        unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            // currentToken->source.printContext("Expected an additive expression");
            return nullptr;
        }
        if (!currentToken || currentToken->getType() != Token::Type::Right_Bracket) {
            currentToken->source.printContext("Expected a right bracket");
            return nullptr;
        }
        children.emplace_back(move(additiveExpr));
        children.emplace_back(make_unique<GenericTokenPTNode>(currentToken->source)); //right bracket
    }
    currentToken = lexer.next();

    return make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::PrimaryExpr,
                                                     move(children));
}
//---------------------------------------------------------------------------
} // namespace pljit::parser
//---------------------------------------------------------------------------