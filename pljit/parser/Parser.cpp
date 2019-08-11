#include "Parser.hpp"

//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
unsigned lengthOfChildren(const std::vector<std::unique_ptr<PTNode>>& children)
// Helper method summing up the length of all the children
{
    unsigned length = 0;
    for (const std::unique_ptr<PTNode>& child: children) {
        length += child->source.getLength();
    }
    return length;
}
//---------------------------------------------------------------------------
pljit_source::SourceReference Parser::childrenSourceReference(const std::vector<std::unique_ptr<PTNode>>& children)
// Creates a new source-reference containing the range of all the passed children
{
    unsigned length = lengthOfChildren(children);
    return pljit_source::SourceReference(children[0]->source.getLineNum(), children[0]->source.getCharPos(), length,
                                         lexer.code);
}
//---------------------------------------------------------------------------
std::unique_ptr<NonTerminalPTNode> Parser::parseFunctionDefinition()
// Public interface to create the parse-treee of the function definition
{
    currentToken = lexer.next();
    if (!currentToken) {
        return nullptr;
    }
    std::unique_ptr<PTNode> paramDeclaration = parseParameterDeclaration();
    if (errorDuringDeclaration) { // parseParameterDeclaration sets this if encountered parsing-error
        return nullptr;
    }
    std::unique_ptr<PTNode> varDeclaration = parseVariableDeclaration();
    if (errorDuringDeclaration) { // parseVariableDeclaration sets this if encountered parsing-error
        return nullptr;
    }
    std::unique_ptr<PTNode> constDeclaration = parseConstDeclaration();
    if (errorDuringDeclaration) { // parseConstDeclaration sets this if encountered parsing-error
        return nullptr;
    }
    std::unique_ptr<PTNode> compoundStatement = parseCompoundStatement();
    if (!compoundStatement) {
        return nullptr;
    }
    if (currentToken->getType() != pljit_lexer::Token::Type::Final) {
        currentToken->source.printContext("Expected final token");
        return nullptr;
    }

    // store parse-tree nodes in children
    std::vector<std::unique_ptr<PTNode>> children;
    if (paramDeclaration) {
        children.emplace_back(std::move(paramDeclaration));
    }
    if (varDeclaration) {
        children.emplace_back(std::move(varDeclaration));
    }
    if (constDeclaration) {
        children.emplace_back(std::move(constDeclaration));
    }
    children.emplace_back(std::move(compoundStatement));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // final token "."

    std::unique_ptr<NonTerminalPTNode> res = std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                                                                 PTNode::Type::FunctionDefinition,
                                                                                 std::move(children));
    return res;
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseParameterDeclaration()
// Return the parse-tree for the parameter-declaration from the token stream
{
    std::vector<std::unique_ptr<PTNode>> children;
    if (!currentToken || currentToken->getType() != pljit_lexer::Token::Type::Param) {
        return nullptr;
    }
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // param keyword

    currentToken = lexer.next();
    if (!currentToken) {
        return nullptr;
    }
    std::unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        return nullptr;
    }
    if (currentToken->getType() != pljit_lexer::Token::Type::Semicolon) {
        currentToken->source.printContext("Expected separator");
        errorDuringDeclaration = true; // to make sure that we abort the current compilation
        return nullptr;
    }

    children.emplace_back(std::move(declaratorList));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator
    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::ParamDeclaration,
                                               std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseVariableDeclaration()
// Return the parse-tree for the variable-declaration from the token stream
{
    std::vector<std::unique_ptr<PTNode>> children;
    if (!currentToken || currentToken->getType() != pljit_lexer::Token::Type::Var) {
        return nullptr;
    }
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // var keyword
    currentToken = lexer.next();
    if (!currentToken) {
        return nullptr;
    }

    std::unique_ptr<PTNode> declaratorList = parseDeclaratorList();
    if (!declaratorList) {
        return nullptr;
    }

    if (!currentToken || currentToken->getType() != pljit_lexer::Token::Type::Semicolon) {
        currentToken->source.printContext("Expected semicolon");
        return nullptr;
    }
    children.emplace_back(std::move(declaratorList));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator

    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::VarDeclaration,
                                               std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseConstDeclaration()
// Return the parse-tree for the const-declaration from the token stream
{
    if (!currentToken || currentToken->getType() != pljit_lexer::Token::Type::Const) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // const keyword
    currentToken = lexer.next();
    if (!currentToken) {
        return nullptr;
    }

    std::unique_ptr<PTNode> declaratorList = parseInitDeclaratorList();
    if (!declaratorList) {
        return nullptr;
    }

    if (!currentToken || currentToken->getType() != pljit_lexer::Token::Type::Semicolon) {
        currentToken->source.printContext("Expected semicolon");
        return nullptr;
    }
    children.emplace_back(std::move(declaratorList));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator
    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::ConstDeclaration,
                                               std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseDeclaratorList()
// Return the parse-tree for the declarator-list from the token stream
{
    if (currentToken->getType() != pljit_lexer::Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        errorDuringDeclaration = true;
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source));

    currentToken = lexer.next();
    while (currentToken && currentToken->getType() == pljit_lexer::Token::Type::Colon) {
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source));

        currentToken = lexer.next();
        if (!currentToken) {
            errorDuringDeclaration = true;
            return nullptr;
        }
        if (currentToken->getType() != pljit_lexer::Token::Type::Identifier) {
            currentToken->source.printContext("Expected identifier");
            errorDuringDeclaration = true;
            return nullptr;
        }
        children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source));
        currentToken = lexer.next();
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::DeclaratorList, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseInitDeclaratorList()
// Return the parse-tree for the init-declarator-list from the token stream
{
    std::unique_ptr<PTNode> initDeclarator = parseInitDeclarator();
    if (!initDeclarator) {
        return nullptr;
    }

    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::move(initDeclarator));

    while (currentToken && currentToken->getType() == pljit_lexer::Token::Type::Colon) { // repeat
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // colon separator
        currentToken = lexer.next();
        if (!currentToken) {
            return nullptr;
        }
        std::unique_ptr<PTNode> declarator = parseInitDeclarator();
        if (!declarator) {
            return nullptr;
        }

        children.emplace_back(std::move(declarator));
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::InitDeclaratorList, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseInitDeclarator()
// Return the parse-tree for the init-declarator from the token stream
{
    if (currentToken->getType() != pljit_lexer::Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        errorDuringDeclaration = true;
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source)); // identifier
    currentToken = lexer.next();
    if (!currentToken) {
        errorDuringDeclaration = true;
        return nullptr;
    }

    if (currentToken->getType() != pljit_lexer::Token::Type::Equal) {
        currentToken->source.printContext("Expected equal sign");
        errorDuringDeclaration = true;
        return nullptr;
    }
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // equal sign

    currentToken = lexer.next();
    if (!currentToken) {
        errorDuringDeclaration = true;
        return nullptr;
    }
    if (currentToken->getType() != pljit_lexer::Token::Type::Literal) {
        currentToken->source.printContext("Expected literal");
        errorDuringDeclaration = true;
        return nullptr;
    }
    children.emplace_back(
            std::make_unique<LiteralPTNode>(currentToken->source,
                                            stoll(std::string(currentToken->source.getText()), nullptr, 10)));
    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::InitDeclarator, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseCompoundStatement()
// Return the parse-tree for the compound-statement from the token stream
{
    if (currentToken->getType() != pljit_lexer::Token::Type::Begin) {
        currentToken->source.printContext("Expected BEGIN keyword");
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // beginKeyword
    currentToken = lexer.next();
    if (!currentToken) {
        return nullptr;
    }

    std::unique_ptr<PTNode> statementList = parseStatementList();
    if (!statementList) {
        return nullptr;
    }
    if (currentToken->getType() != pljit_lexer::Token::Type::End) {
        currentToken->source.printContext("Expected END keyword");
        return nullptr;
    }

    children.emplace_back(std::move(statementList));
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // end keyword
    currentToken = lexer.next();
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::CompoundStatement, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseStatementList()
// Return the parse-tree for the statement-list from the token stream
{
    std::unique_ptr<PTNode> statement = parseStatement();
    if (!statement) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::move(statement));

    while (currentToken && currentToken->getType() == pljit_lexer::Token::Type::Semicolon) { // repeat
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // semicolon separator
        currentToken = lexer.next();
        if (!currentToken) {
            return nullptr;
        }

        std::unique_ptr<PTNode> statementRepeat = parseStatement();
        if (!statementRepeat) {
            return nullptr;
        }
        children.emplace_back(std::move(statementRepeat));
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::StatementList, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseStatement()
// Return the parse-tree for the statement from the token stream
{
    std::vector<std::unique_ptr<PTNode>> children;

    if (currentToken && currentToken->getType() == pljit_lexer::Token::Type::Return) {
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source));
        currentToken = lexer.next();
        if (!currentToken) {
            return nullptr;
        }
        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            return nullptr;
        }

        children.emplace_back(std::move(additiveExpr));
    } else {
        std::unique_ptr<PTNode> assignmentExpr = parseAssignmentExpr();
        if (!assignmentExpr) {
            return nullptr;
        }
        children.emplace_back(std::move(assignmentExpr));
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::Statement, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseAssignmentExpr()
// Return the parse-tree for the assignment-expression from the token stream
{
    if (currentToken->getType() != pljit_lexer::Token::Type::Identifier) {
        currentToken->source.printContext("Expected identifier");
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source)); // identifier

    currentToken = lexer.next();
    if (!currentToken) {
        return nullptr;
    }
    if (currentToken->getType() != pljit_lexer::Token::Type::Assignment) {
        currentToken->source.printContext("Expected assignment");
        return nullptr;
    }
    children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // assignment
    currentToken = lexer.next();
    if (!currentToken) {
        return nullptr;
    }
    std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
    if (!additiveExpr) {
        return nullptr;
    }

    children.emplace_back(std::move(additiveExpr));
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::AssignmentExpr, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseAdditiveExpr()
// Return the parse-tree for an additive-expression from the token stream
{
    std::unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
    if (!multiplicativeExpr) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::move(multiplicativeExpr));

    if (currentToken && (currentToken->getType() == pljit_lexer::Token::Type::Plus ||
                         currentToken->getType() == pljit_lexer::Token::Type::Minus)) {

        OperatorAlternationPTNode::OperatorType type;
        if (currentToken->getType() == pljit_lexer::Token::Type::Plus) {
            type = OperatorAlternationPTNode::Plus;
        } else {
            type = OperatorAlternationPTNode::Minus;
        }
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, type));

        currentToken = lexer.next();
        if(!currentToken) {
            return nullptr;
        }
        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            return nullptr;
        }
        children.emplace_back(std::move(additiveExpr));
    }
    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::AdditiveExpr, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseMultiplicativeExpr()
// Return the parse-tree for a multiplicative-expression from the token stream
{
    std::unique_ptr<PTNode> unaryExpr = parseUnaryExpr();
    if (!unaryExpr) {
        return nullptr;
    }
    std::vector<std::unique_ptr<PTNode>> children;
    children.emplace_back(std::move(unaryExpr));
    if (currentToken && (currentToken->getType() == pljit_lexer::Token::Type::Multiply ||
                         currentToken->getType() == pljit_lexer::Token::Type::Divide)) {

        OperatorAlternationPTNode::OperatorType type;
        if (currentToken->getType() == pljit_lexer::Token::Type::Multiply) {
            type = OperatorAlternationPTNode::Multiply;
        } else {
            type = OperatorAlternationPTNode::Divide;
        }
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, type));
        currentToken = lexer.next();
        if(!currentToken) {
            return nullptr;
        }
        std::unique_ptr<PTNode> multiplicativeExpr = parseMultiplicativeExpr();
        if (!multiplicativeExpr) {
            return nullptr;
        }
        children.emplace_back(std::move(multiplicativeExpr));
    }
    std::unique_ptr<NonTerminalPTNode> res = std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                                                                 PTNode::Type::MultiplicativeExpr,
                                                                                 std::move(children));
    return res;
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parseUnaryExpr()
// Return the parse-tree for an unary-expression from the token stream
{
    std::vector<std::unique_ptr<PTNode>> children;
    if (currentToken && currentToken->getType() == pljit_lexer::Token::Type::Plus) { // optional
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Plus));
        currentToken = lexer.next();
    } else if (currentToken && currentToken->getType() == pljit_lexer::Token::Type::Minus) { // optional
        children.emplace_back(
                std::make_unique<OperatorAlternationPTNode>(currentToken->source, OperatorAlternationPTNode::Minus));
        currentToken = lexer.next();
    }

    if(!currentToken) {
        return nullptr;
    }
    std::unique_ptr<PTNode> primaryExpr = parsePrimaryExpr();
    if (!primaryExpr) {
        return nullptr;
    }
    children.emplace_back(std::move(primaryExpr));


    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children),
                                               PTNode::Type::UnaryExpr, std::move(children));
}
//---------------------------------------------------------------------------
std::unique_ptr<PTNode> Parser::parsePrimaryExpr()
// Return the parse-tree for a primary-expression from the token stream
{
    std::vector<std::unique_ptr<PTNode>> children;
    if(!currentToken) {
        return nullptr;
    }
    if (currentToken->getType() == pljit_lexer::Token::Type::Identifier) {
        children.emplace_back(std::make_unique<IdentifierPTNode>(currentToken->source));
    } else if (currentToken->getType() == pljit_lexer::Token::Type::Literal) {
        children.emplace_back(std::make_unique<LiteralPTNode>(currentToken->source,
                                                              stoll(std::string(currentToken->source.getText()),
                                                                    nullptr, 10)));
    } else if (currentToken->getType() == pljit_lexer::Token::Type::Left_Bracket) {
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); // left bracket
        std::unique_ptr<pljit_lexer::Token> leftBracketToken = std::move(
                currentToken); // potentially needed for error message later
        currentToken = lexer.next();
        if(!currentToken) {
            return nullptr;
        }

        std::unique_ptr<PTNode> additiveExpr = parseAdditiveExpr();
        if (!additiveExpr) {
            return nullptr;
        }
        if (currentToken && currentToken->getType() != pljit_lexer::Token::Type::Right_Bracket) {
            currentToken->source.printContext("Expected ')'");
            leftBracketToken->source.printContext("to match this '('");
            return nullptr;
        }
        children.emplace_back(std::move(additiveExpr));
        children.emplace_back(std::make_unique<GenericTokenPTNode>(currentToken->source)); //right bracket
    } else {
        currentToken->source.printContext("Expected an identifer, literal or '('");
        return nullptr;
    }
    currentToken = lexer.next();

    return std::make_unique<NonTerminalPTNode>(childrenSourceReference(children), PTNode::Type::PrimaryExpr,
                                               std::move(children));
}
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------