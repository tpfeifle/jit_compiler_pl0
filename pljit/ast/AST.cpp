#include "AST.hpp"
#include <unordered_map>
//---------------------------------------------------------------------------
using namespace std;
using namespace pljit::parser;
//---------------------------------------------------------------------------
namespace pljit::ast {
//---------------------------------------------------------------------------
unique_ptr<FunctionAST> AST::analyzeParseTree(shared_ptr<PTNode> root)
// analyze the ParseTree: This is the starting-point for the analysis
{
    auto* node = static_cast<NonTerminalPTNode*>(root.get());
    vector<unique_ptr<ASTNode>> children;
    unsigned childrenIndex = 0;
    if (node->children[0]->getType() == PTNode::Type::ParamDeclaration) {
        auto* paramDeclaration = static_cast<NonTerminalPTNode*>(node->children[0].get());
        analyzeDeclarations(paramDeclaration,
                            Symbol::Type::Param);
        childrenIndex++;
    }
    if (node->children[childrenIndex]->getType() == PTNode::Type::VarDeclaration) {
        auto* varDeclaration = static_cast<NonTerminalPTNode*>(node->children[childrenIndex].get());
        analyzeDeclarations(varDeclaration,
                            Symbol::Type::Var);
        childrenIndex++;
    }
    if (node->children[childrenIndex]->getType() == PTNode::Type::ConstDeclaration) {
        auto* constDeclaration = static_cast<NonTerminalPTNode*>(node->children[childrenIndex].get());
        analyzeConstDeclarations(constDeclaration,Symbol::Type::Const);
        childrenIndex++;
    }

    auto* compoundStatement = static_cast<NonTerminalPTNode*>(node->children[childrenIndex].get());
    if(!compoundStatement) {
        return nullptr;
    }
    auto* statementList = static_cast<NonTerminalPTNode*>(compoundStatement->children[1].get());
    if(!statementList) {
        return nullptr;
    }
    bool hasReturn = false;
    for (unsigned i = 0; i < statementList->children.size(); i++) {
        auto* statement = static_cast<NonTerminalAlternationPTNode*>(statementList->children[i].get());
        // TODO: using .get or .release?
        // TODO: this casting is really ugly ...
        unique_ptr<StatementAST> ASTstatement = unique_ptr<StatementAST>(static_cast<StatementAST*>(analyzeStatement(statement).release()));
        if(!ASTstatement) {
            return nullptr;
        }
        if (ASTstatement->getType() == ASTNode::Type::ReturnStatement) {
            hasReturn = true;
        }
        children.emplace_back(move(ASTstatement));
        i++;
    }
    if (!hasReturn) {
        cerr << "Function has no return statement" << endl;
        // exit(-1); TODO
    }

    return make_unique<FunctionAST>(move(children));
}
//---------------------------------------------------------------------------
void AST::analyzeDeclarations(NonTerminalPTNode* node, Symbol::Type type) {
    // TODO: maybe do not use a simple pointer (always use unique/shared ptr)
    auto* declaratorList = static_cast<NonTerminalPTNode*>(node->children[1].get());
    for (unsigned i = 0; i < declaratorList->children.size(); i++) {
        if (declaratorList->children[i]->getType() == PTNode::Type::Identifier) {
            // for declarator
            auto* identiferToken = static_cast<IdentifierPTNode*>(declaratorList->children[i].get());
            string identifier = identiferToken->getName();
            if (symbolTable.find(identifier) == symbolTable.end()) {
                bool initialize = type == Symbol::Type::Param;
                Symbol symbol = Symbol(type, declaratorList->children[i]->source, initialize, 0);
                symbolTable.emplace(pair<string, pair<Symbol, unsigned>>(identifier, {symbol, currentSymbolId++}));
            } else {
                identiferToken->source.printContext(
                        "This identifier is already declared. Duplicate declarations are not allowed");
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------
void AST::analyzeConstDeclarations(NonTerminalPTNode* node, Symbol::Type type) {
    auto* initDeclaratorList = static_cast<NonTerminalPTNode*>(node->children[1].get());
    for (unsigned i = 0; i < initDeclaratorList->children.size(); i++) {
        auto* initDeclarator = static_cast<NonTerminalPTNode*>(initDeclaratorList->children[i].get());

        auto* identiferToken = static_cast<IdentifierPTNode*>(initDeclarator->children[0].get());
        auto* literal = static_cast<LiteralPTNode*>(initDeclarator->children[2].get());
        string identifier = identiferToken->getName();
        Symbol symbol = Symbol(type, initDeclarator->children[i]->source, true, literal->getValue());
        if (symbolTable.find(identifier) == symbolTable.end()) {
            symbolTable.emplace(pair<string, pair<Symbol, unsigned>>(identifier, {symbol, currentSymbolId++}));
        } else {
            identiferToken->source.printContext(
                    "This identifier is already declared. Duplicate declarations are not allowed");
            return;
        }
        i++;
    }
}
//---------------------------------------------------------------------------
unique_ptr<StatementAST> AST::analyzeStatement(NonTerminalPTNode* node) {
    auto* statement = static_cast<NonTerminalAlternationPTNode*>(node);
    if (statement->chosenAlternation == PTNode::Type::AssignmentExpr) {
        auto* assignmentExpr = static_cast<NonTerminalPTNode*>(statement->children[0].get());
        auto* identiferToken = static_cast<IdentifierPTNode*>(assignmentExpr->children[0].get());
        string identifier = identiferToken->getName();
        if (symbolTable.find(identifier) == symbolTable.end()) {
            identiferToken->source.printContext("Using an undeclared identifier");
            return nullptr;
            // exit(-1);
        }
        if (symbolTable.at(identifier).first.type == Symbol::Type::Const) {
            identiferToken->source.printContext("Assigning a value to a constant");
            return nullptr;
            // exit(-1);
        }
        symbolTable.at(identifier).first.initialized = true;
        auto* additiveExpr = static_cast<NonTerminalPTNode*>(assignmentExpr->children[2].get());
        return make_unique<AssignmentAST>(make_unique<IdentifierAST>(identifier), analyzeAdditiveExpression(additiveExpr));
        // this has to be stored in context with identifier
    } else if (statement->chosenAlternation == PTNode::Type::AdditiveExpr) {
        auto* additiveExprStatement = static_cast<NonTerminalPTNode*>(statement->children[1].get());
        // this is return
        return make_unique<ReturnStatementAST>(analyzeAdditiveExpression(additiveExprStatement));
    } else {
        // not expecting this
        cout << "FOOO should not happen" << endl;
        return nullptr;
        // exit(-1);
    }
}
//---------------------------------------------------------------------------
unique_ptr<ExpressionAST> AST::analyzeAdditiveExpression(NonTerminalPTNode* node) {
    if (node->children.size() == 1) {
        return analyzeMultiplicativeExpression(static_cast<NonTerminalPTNode*>(node->children[0].get()));
    } else {
        // handle plus minus
        auto left = analyzeMultiplicativeExpression(static_cast<NonTerminalPTNode*>(node->children[0].get()));
        auto* additiveOperator = static_cast<OperatorAlternationPTNode*>(node->children[1].get());
        auto right = analyzeAdditiveExpression(static_cast<NonTerminalPTNode*>(node->children[2].get()));

        if (additiveOperator->getOperatorType() == OperatorAlternationPTNode::Plus) {
            return make_unique<BinaryOperationAST>(move(left), BinaryOperationAST::Plus, move(right));
        } else {
            return make_unique<BinaryOperationAST>(move(left), BinaryOperationAST::Minus, move(right));
        }

    }
}
//---------------------------------------------------------------------------
unique_ptr<ExpressionAST> AST::analyzeMultiplicativeExpression(NonTerminalPTNode* node) {
    auto* unaryExpr = static_cast<NonTerminalPTNode*>(node->children[0].get());
    unsigned unaryChildIndex = 0;
    if (unaryExpr->children.size() == 2) {
        unaryChildIndex++;
    }
    // TODO regard this signOperator in the left somehow

    auto* primaryExpr = static_cast<NonTerminalAlternationPTNode*>(unaryExpr->children[unaryChildIndex].get());
    unique_ptr<ExpressionAST> left;
    if (primaryExpr->chosenAlternation == PTNode::Type::Identifier) {
        auto* identiferToken = static_cast<IdentifierPTNode*>(primaryExpr->children[0].get());
        string identifier = identiferToken->getName();
        if (symbolTable.find(identifier) == symbolTable.end()) {
            identiferToken->source.printContext("Using an undeclared identifier");
            return nullptr;
            // exit(-1);
        }
        if (!symbolTable.at(identifier).first.initialized) {
            identiferToken->source.printContext("Using an uninitialized identifier");
            return nullptr;
            // exit(-1);
        }
        left = make_unique<IdentifierAST>(identifier);
    } else if (primaryExpr->chosenAlternation == PTNode::Type::Literal) {
        auto* literal = static_cast<LiteralPTNode*>(primaryExpr->children[0].get());
        left = make_unique<LiteralAST>(literal->getValue());
    } else {
        // TODO: doe we need to handle the brackets? I guess so?
        if(primaryExpr->children.size() == 3) { // has brackets
            left = analyzeAdditiveExpression(static_cast<NonTerminalPTNode*>(primaryExpr->children[1].get()));
        } else { // no brackets
            left = analyzeAdditiveExpression(static_cast<NonTerminalPTNode*>(primaryExpr->children[0].get()));
        }

    }

    if (unaryChildIndex == 1) { //this means we had a plus or minus optional in the unary expression
        auto* signOperator = static_cast<OperatorAlternationPTNode*>(unaryExpr->children[0].get());
        if (signOperator->getOperatorType() == OperatorAlternationPTNode::Plus) {
            left = make_unique<UnaryAST>(move(left), UnaryAST::SignType::Plus);
        } else {
            left = make_unique<UnaryAST>(move(left), UnaryAST::SignType::Minus);
        }

    }

    if (node->children.size() == 3) {
        auto* muldivOperator = static_cast<OperatorAlternationPTNode*>(node->children[1].get());
        BinaryOperationAST::OperationType operatorType;
        if (muldivOperator->getOperatorType() == OperatorAlternationPTNode::Multiply) {
            operatorType = BinaryOperationAST::Multiply;
        } else {
            operatorType = BinaryOperationAST::Divide;
        }
        // TODO need to know the choice of plus or minus
        auto right = analyzeMultiplicativeExpression(static_cast<NonTerminalPTNode*>(node->children[2].get()));
        return make_unique<BinaryOperationAST>(move(left), operatorType, move(right));
    }

    return left; // only unary-expression
}
//---------------------------------------------------------------------------
} // namespace pljit::ast
//---------------------------------------------------------------------------