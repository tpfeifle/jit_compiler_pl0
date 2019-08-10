#include "Parser.hpp"

//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
unsigned lengthOfChildren(const std::vector<std::unique_ptr<PTNode>>& children) {
    unsigned length = 0;
    for (const std::unique_ptr<PTNode>& child: children) {
        length += child->source.getLength();
    }
    return length;
}
//---------------------------------------------------------------------------
pljit_source::SourceReference Parser::childrenSourceReference(const std::vector<std::unique_ptr<PTNode>>& children) {
    unsigned length = lengthOfChildren(children);
    return pljit_source::SourceReference(children[0]->source.getLineNum(), children[0]->source.getCharPos(), length,
                                         lexer.code);
}
//---------------------------------------------------------------------------
std::unique_ptr<NonTerminalPTNode> Parser::parseFunctionDefinition() {
    currentToken = lexer.next();
    std::unique_ptr<PTNode> paramDeclaration = parseParameterDeclaration();
    if(errorDuringDeclaration) {
        return nullptr;
    }
    std::unique_ptr<PTNode> varDeclaration = parseVariableDeclaration();
    if(errorDuringDeclaration) {
        return nullptr;
    }
    std::unique_ptr<PTNode> constDeclaration = parseConstDeclaration();
    if(errorDuringDeclaration) {
        return nullptr;
    }
    std::unique_ptr<PTNode> compoundStatement = parseCompoundStatement();
    if (!compoundStatement) {
        return nullptr;
    }
    if (currentToken->getType() != pljit_lexer::Token::Type::Final) {
        std::cout << "Expected Final token" << std::endl;
        return nullptr;
    }

    std::vector<std::unique_ptr<PTNode>> children;
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
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // final token "."

    std::unique_ptr<NonTerminalPTNode> res =  std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::FunctionDefinition,
                                               move(children));
    return res;
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseParameterDeclaration() {
    if (currentToken->getType() != pljit_lexer::Token::Type::Param) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // param keyword

    currentToken = lexer.next();
    std::unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        return nullptr;
    }
    if (currentToken->getType() != pljit_lexer::Token::Type::Semicolon) {
        currentToken->source.printContext("Expected separator");
        errorDuringDeclaration = true;
        return nullptr;
    }

    children.emplace_back(move(declaratorList));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator

    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::ParamDeclaration,
                                               move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseVariableDeclaration() {
    std::vector<std::unique_ptr<PTNode>> children;
    if (currentToken->getType() != pljit_lexer::Token::Type::Var) {
        return nullptr;
    }
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // var keyword
    currentToken = lexer.next();

    std::unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        return nullptr;
    }

    if (currentToken->getType() != pljit_lexer::Token::Type::Semicolon) {
        currentToken->source.printContext("Expected semicolon");
        return nullptr;
    }
    children.emplace_back(move(declaratorList));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator

    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::VarDeclaration,
                                               move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseConstDeclaration() {
    if (currentToken->getType() != pljit_lexer::Token::Type::Const) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // const keyword
    currentToken = lexer.next();

    std::unique_ptr<PTNode> declaratorList = parseInitDeclaratorList();
    if (!declaratorList) {
        return nullptr;
    }

    if (currentToken->getType() != pljit_lexer::Token::Type::Semicolon) {
        currentToken->source.printContext("Expected semicolon");
        return nullptr;
    }
    children.emplace_back(move(declaratorList));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator
    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::ConstDeclaration,
                                               move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseDeclaratorList() {
    if (currentToken->getType() != pljit_lexer::Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        errorDuringDeclaration = true;
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source));

    currentToken = lexer.next();
    while (currentToken->getType() == pljit_lexer::Token::Type::Colon) {
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source));

        currentToken = lexer.next();
        if (currentToken->getType() != pljit_lexer::Token::Type::Identifier) {
            currentToken->source.printContext("Expected identifier");
            errorDuringDeclaration = true;
            return nullptr;
        }
        children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source));
        currentToken = lexer.next();
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::DeclaratorList, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseInitDeclaratorList() {
    std::unique_ptr<PTNode> initDeclarator = parseInitDeclarator();
    if (!initDeclarator) {
        return nullptr;
    }

    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(move(initDeclarator));

    while (currentToken->getType() == pljit_lexer::Token::Type::Colon) { // repeat
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // colon separator
        currentToken = lexer.next();
        std::unique_ptr<PTNode> declarator = parseInitDeclarator();
        if (!declarator) {
            return nullptr;
        }

        children.emplace_back(move(declarator));
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::InitDeclaratorList, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseInitDeclarator() {
    if (currentToken->getType() != pljit_lexer::Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        errorDuringDeclaration = true;
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source)); // identifier
    currentToken = lexer.next();

    if (currentToken->getType() != pljit_lexer::Token::Type::Equal) {
        currentToken->source.printContext("Expected equal sign");
        errorDuringDeclaration = true;
        return nullptr;
    }
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // equal sign

    currentToken = lexer.next();
    if (currentToken->getType() != pljit_lexer::Token::Type::Literal) {
        currentToken->source.printContext("Expected literal");
        errorDuringDeclaration = true;
        return nullptr;
    }
    children.emplace_back(
            std::make_unique<LiteralPTNode>(currentToken->source, stoll(currentToken->source.getText(), nullptr, 10)));
    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::InitDeclarator, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseCompoundStatement() {
    if (currentToken->getType() != pljit_lexer::Token::Type::Begin) {
        currentToken->source.printContext("Expected BEGIN keyword");
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // beginKeyword
    currentToken = lexer.next();

    std::unique_ptr<PTNode> statementList = parseStatementList();
    if (!statementList) {
        return nullptr;
    }
    if (currentToken->getType() != pljit_lexer::Token::Type::End) {
        currentToken->source.printContext("Expected END keyword");
        return nullptr;
    }

    children.emplace_back(move(statementList));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // end keyword
    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::CompoundStatement, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseStatementList() {
    std::unique_ptr<PTNode> statement = parseStatement();
    if (!statement) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(move(statement));

    while (currentToken->getType() == pljit_lexer::Token::Type::Semicolon) { // repeat
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator
        currentToken = lexer.next();

        std::unique_ptr<PTNode> statementRepeat = parseStatement();
        if (!statementRepeat) {
            return nullptr;
        }
        children.emplace_back(move(statementRepeat));
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::StatementList, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseStatement() {
    std::vector<std::unique_ptr<PTNode>> children;

    if (currentToken->getType() == pljit_lexer::Token::Type::Return) {
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source));
        currentToken = lexer.next();
        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            return nullptr;
        }

        children.emplace_back(move(additiveExpr));
    } else {
        std::unique_ptr<PTNode> assignmentExpr = parseAssignmentExpr();
        if (!assignmentExpr) {
            return nullptr;
        }
        children.emplace_back(move(assignmentExpr));
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::Statement, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseAssignmentExpr() {
    if (currentToken->getType() != pljit_lexer::Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source)); // identifier

    currentToken = lexer.next();
    if (currentToken->getType() != pljit_lexer::Token::Type::Assignment) {
        currentToken->source.printContext("Expected assignment");
        return nullptr;
    }
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // assignment
    currentToken = lexer.next();

    std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
    if (!additiveExpr) {
        return nullptr;
    }

    children.emplace_back(move(additiveExpr));
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::AssignmentExpr, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseAdditiveExpr() {
    std::unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
    if (!multiplicativeExpr) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(move(multiplicativeExpr));

    if (currentToken->getType() == pljit_lexer::Token::Type::Plus) {
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Plus));
        currentToken = lexer.next();
        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            return nullptr;
        }
        children.emplace_back(move(additiveExpr));
    } else if (currentToken->getType() == pljit_lexer::Token::Type::Minus) {
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Minus));
        currentToken = lexer.next();
        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            return nullptr;
        }
        children.emplace_back(move(additiveExpr));
    }

    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::AdditiveExpr, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseMultiplicativeExpr() {
    std::unique_ptr<PTNode> unaryExpr = parseUnaryExpr();
    if (!unaryExpr) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(move(unaryExpr));
    if (currentToken->getType() == pljit_lexer::Token::Type::Multiply) { // optional
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Multiply));
        currentToken = lexer.next();
        std::unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
        if (!multiplicativeExpr) {
            return nullptr;
        }
        children.emplace_back(move(multiplicativeExpr));
    } else if (currentToken->getType() == pljit_lexer::Token::Type::Divide) { // optional
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Divide));
        currentToken = lexer.next();
        std::unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
        if (!multiplicativeExpr) {
            return nullptr;
        }
        children.emplace_back(move(multiplicativeExpr));
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::MultiplicativeExpr, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseUnaryExpr() {
    std::vector<std::unique_ptr<PTNode>> children;
    if (currentToken->getType() == pljit_lexer::Token::Type::Plus) { // optional
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Plus));
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source));
        currentToken = lexer.next();
    } else if (currentToken->getType() == pljit_lexer::Token::Type::Minus) { // optional
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Minus));
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source));
        currentToken = lexer.next();
    }

    std::unique_ptr<PTNode> primaryExpr = parsePrimaryExpr();
    if (!primaryExpr) {
        return nullptr;
    }
    children.emplace_back(move(primaryExpr));


    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::UnaryExpr, move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parsePrimaryExpr() {
    std::vector<std::unique_ptr<PTNode>> children;
    if (currentToken->getType() == pljit_lexer::Token::Type::Identifier) {
        children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source));
    } else if (currentToken->getType() == pljit_lexer::Token::Type::Literal) {
        children.emplace_back(std::make_unique<LiteralPTNode>(currentToken->source,
                                                              stoll(currentToken->source.getText(),
                                                                    nullptr, 10)));
    } else if (currentToken->getType() == pljit_lexer::Token::Type::Left_Bracket) {
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // left bracket
        currentToken = lexer.next();

        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            return nullptr;
        }
        if (currentToken->getType() != pljit_lexer::Token::Type::Right_Bracket) {
            currentToken->source.printContext("Expected a right bracket");
            return nullptr;
        }
        children.emplace_back(move(additiveExpr));
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); //right bracket
    } else {
        currentToken->source.printContext("Expected an Identifer, Literal or Left-Bracket");
        return nullptr;
    }
    currentToken = lexer.next();

    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::PrimaryExpr,
                                               move(children));
}
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------