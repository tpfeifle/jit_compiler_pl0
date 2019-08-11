#include "SemanticAnalyzer.hpp"
#include <unordered_map>
//---------------------------------------------------------------------------
using namespace pljit_parser;
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
std::unique_ptr<FunctionAST> SemanticAnalyzer::analyzeParseTree(std::unique_ptr<NonTerminalPTNode> node)
// analyze the ParseTree: This is the starting-point for the analysis
{
    std::vector<std::unique_ptr<StatementAST>> children;
    unsigned childrenIndex = 0;
    if (node->children[0]->getType() == PTNode::Type::ParamDeclaration) {
        auto* paramDeclaration = static_cast<NonTerminalPTNode*>(node->children[0].get());
        analyzeDeclarations(paramDeclaration, Symbol::Type::Param);
        childrenIndex++;
    }
    if (node->children[childrenIndex]->getType() == PTNode::Type::VarDeclaration) {
        auto* varDeclaration = static_cast<NonTerminalPTNode*>(node->children[childrenIndex].get());
        analyzeDeclarations(varDeclaration, Symbol::Type::Var);
        childrenIndex++;
    }
    if (node->children[childrenIndex]->getType() == PTNode::Type::ConstDeclaration) {
        auto* constDeclaration = static_cast<NonTerminalPTNode*>(node->children[childrenIndex].get());
        analyzeConstDeclarations(constDeclaration, Symbol::Type::Const);
        childrenIndex++;
    }

    auto* compoundStatement = static_cast<NonTerminalPTNode*>(node->children[childrenIndex].get());
    if (!compoundStatement) {
        return nullptr;
    }
    auto* statementList = static_cast<NonTerminalPTNode*>(compoundStatement->children[1].get());
    if (!statementList) {
        return nullptr;
    }
    bool hasReturn = false;
    for (unsigned long i = 0; i < statementList->children.size(); i += 2) {
        auto* statement = static_cast<NonTerminalPTNode*>(statementList->children[i].get());
        std::unique_ptr<StatementAST> ASTstatement = std::unique_ptr<StatementAST>(analyzeStatement(statement).release());
        if (!ASTstatement) {
            return nullptr;
        }
        if (ASTstatement->getType() == ASTNode::Type::ReturnStatement) {
            hasReturn = true;
        }
        children.emplace_back(std::move(ASTstatement));
    }
    if (!hasReturn) {
        std::cerr << "Function has no return statement" << std::endl;
    }
    std::unique_ptr<FunctionAST> res = std::make_unique<FunctionAST>(std::move(children));
    return res;
}
//---------------------------------------------------------------------------
void SemanticAnalyzer::analyzeDeclarations(NonTerminalPTNode* node, Symbol::Type type) {
    auto* declaratorList = static_cast<NonTerminalPTNode*>(node->children[1].get());
    for (auto& child: declaratorList->children) {
        if (child->getType() == PTNode::Type::Identifier) {
            // for declarator
            auto* identiferToken = static_cast<IdentifierPTNode*>(child.get());
            std::string identifier = identiferToken->getName();
            if (symbolTable.find(identifier) == symbolTable.end()) {
                bool initialize = type != Symbol::Type::Var;
                Symbol symbol = Symbol(type, child->source, initialize, 0);
                symbolTable.emplace(
                        std::pair<std::string, std::pair<Symbol, unsigned>>(identifier, {symbol, symbolTable.size()}));
            } else {
                identiferToken->source.printContext(
                        "This identifier is already declared. Duplicate declarations are not allowed");
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------
void SemanticAnalyzer::analyzeConstDeclarations(NonTerminalPTNode* node, Symbol::Type type) {
    auto* initDeclaratorList = static_cast<NonTerminalPTNode*>(node->children[1].get());
    for (unsigned long i = 0; i < initDeclaratorList->children.size(); i += 2) {
        auto* initDeclarator = static_cast<NonTerminalPTNode*>(initDeclaratorList->children[i].get());

        auto* identiferToken = static_cast<IdentifierPTNode*>(initDeclarator->children[0].get());
        auto* literal = static_cast<LiteralPTNode*>(initDeclarator->children[2].get());
        std::string identifier = identiferToken->getName();
        Symbol symbol = Symbol(type, initDeclarator->children[i]->source, true, literal->getValue());
        if (symbolTable.find(identifier) == symbolTable.end()) {
            symbolTable.emplace(
                    std::pair<std::string, std::pair<Symbol, unsigned>>(identifier, {symbol, symbolTable.size()}));
        } else {
            identiferToken->source.printContext(
                    "This identifier is already declared. Duplicate declarations are not allowed");
            return;
        }
    }
}
//---------------------------------------------------------------------------
std::unique_ptr<StatementAST> SemanticAnalyzer::analyzeStatement(NonTerminalPTNode* node) {
    auto* statement = static_cast<NonTerminalPTNode*>(node);
    if (statement->children.size() == 1) {
        // assignment statement
        auto* assignmentExpr = static_cast<NonTerminalPTNode*>(statement->children[0].get());
        auto* identiferToken = static_cast<IdentifierPTNode*>(assignmentExpr->children[0].get());
        std::string identifier = identiferToken->getName();
        if (symbolTable.find(identifier) == symbolTable.end()) {
            identiferToken->source.printContext("Using an undeclared identifier");
            return nullptr;
        }
        if (symbolTable.at(identifier).first.type == Symbol::Type::Const) {
            identiferToken->source.printContext("Assigning a value to a constant");
            return nullptr;
        }
        symbolTable.at(identifier).first.initialized = true;
        auto* additiveExpr = static_cast<NonTerminalPTNode*>(assignmentExpr->children[2].get());
        return std::make_unique<AssignmentAST>(std::make_unique<IdentifierAST>(identifier),
                                               analyzeAdditiveExpression(additiveExpr));
    } else {
        // return statement
        auto* additiveExprStatement = static_cast<NonTerminalPTNode*>(statement->children[1].get());
        return std::make_unique<ReturnStatementAST>(analyzeAdditiveExpression(additiveExprStatement));
    }
}
//---------------------------------------------------------------------------
std::unique_ptr<ExpressionAST> SemanticAnalyzer::analyzeAdditiveExpression(NonTerminalPTNode* node) {
    if (node->children.size() == 1) {
        // only multiplicative expression
        return analyzeMultiplicativeExpression(static_cast<NonTerminalPTNode*>(node->children[0].get()));
    } else {
        // handle plus minus
        auto left = analyzeMultiplicativeExpression(static_cast<NonTerminalPTNode*>(node->children[0].get()));
        auto* additiveOperator = static_cast<OperatorAlternationPTNode*>(node->children[1].get());
        auto right = analyzeAdditiveExpression(static_cast<NonTerminalPTNode*>(node->children[2].get()));

        if (additiveOperator->getOperatorType() == OperatorAlternationPTNode::Plus) {
            return std::make_unique<BinaryOperationAST>(std::move(left), BinaryOperationAST::Plus, std::move(right));
        } else {
            return std::make_unique<BinaryOperationAST>(std::move(left), BinaryOperationAST::Minus, std::move(right));
        }

    }
}
//---------------------------------------------------------------------------
std::unique_ptr<ExpressionAST> SemanticAnalyzer::analyzeMultiplicativeExpression(NonTerminalPTNode* node) {
    auto* unaryExpr = static_cast<NonTerminalPTNode*>(node->children[0].get());
    unsigned primaryExprIndex = 0;
    if (unaryExpr->children.size() == 2) {
        primaryExprIndex++;
    }

    // Analyze Primary Expression
    auto* primaryExpr = static_cast<NonTerminalPTNode*>(unaryExpr->children[primaryExprIndex].get());
    std::unique_ptr<ExpressionAST> left;
    if (primaryExpr->children[0]->getType() == PTNode::Type::Identifier) {
        auto* identiferToken = static_cast<IdentifierPTNode*>(primaryExpr->children[0].get());
        std::string identifier = identiferToken->getName();
        if (symbolTable.find(identifier) == symbolTable.end()) {
            identiferToken->source.printContext("Using an undeclared identifier");
            return nullptr;
        }
        if (!symbolTable.at(identifier).first.initialized) {
            identiferToken->source.printContext("Using an uninitialized identifier");
            return nullptr;
        }
        left = std::make_unique<IdentifierAST>(identifier);
    } else if (primaryExpr->children[0]->getType() == PTNode::Type::Literal) {
        auto* literal = static_cast<LiteralPTNode*>(primaryExpr->children[0].get());
        left = std::make_unique<LiteralAST>(literal->getValue());
    } else {
        if (primaryExpr->children.size() == 3) { // has brackets
            left = analyzeAdditiveExpression(static_cast<NonTerminalPTNode*>(primaryExpr->children[1].get()));
        } else { // no brackets
            left = analyzeAdditiveExpression(static_cast<NonTerminalPTNode*>(primaryExpr->children[0].get()));
        }

    }

    // Parse Unary Expression
    if (primaryExprIndex == 1) { //this means we had a plus or minus optional in the unary expression
        auto* signOperator = static_cast<OperatorAlternationPTNode*>(unaryExpr->children[0].get());
        if (signOperator->getOperatorType() == OperatorAlternationPTNode::Plus) {
            left = std::make_unique<UnaryAST>(std::move(left), UnaryAST::SignType::Plus);
        } else {
            left = std::make_unique<UnaryAST>(std::move(left), UnaryAST::SignType::Minus);
        }

    }

    // optionally additional multiplicative expression (right side of expression)
    if (node->children.size() == 3) {
        auto* muldivOperator = static_cast<OperatorAlternationPTNode*>(node->children[1].get());
        BinaryOperationAST::OperationType operatorType;
        if (muldivOperator->getOperatorType() == OperatorAlternationPTNode::Multiply) {
            operatorType = BinaryOperationAST::Multiply;
        } else {
            operatorType = BinaryOperationAST::Divide;
        }

        auto right = analyzeMultiplicativeExpression(static_cast<NonTerminalPTNode*>(node->children[2].get()));
        return std::make_unique<BinaryOperationAST>(std::move(left), operatorType, std::move(right));
    } else {
        // if only unary expression
        return left;
    }
}
//---------------------------------------------------------------------------
} // namespace pljit_ast
//---------------------------------------------------------------------------