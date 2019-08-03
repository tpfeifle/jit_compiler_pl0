#include "AST.hpp"


std::unique_ptr<ASTNode> AST::analyzeParseTree(std::shared_ptr<PTNode> root) {
    if((*root).children[0].type == PTNode::Type::ParamDeclaration) {
        std::unique_ptr<ASTNode> foo = analyzeDeclarations(&(*root).children[0].children[1], Symbol::Type::Param); // analyze declarator list
    }
    //TODO check better, e.g. whether even two children, ...
    if((*root).children[1].type == PTNode::Type::ParamDeclaration) {
        std::unique_ptr<ASTNode> foo = analyzeDeclarations(&(*root).children[0].children[1], Symbol::Type::Var); // analyze declarator list
    }
    return nullptr;
}

// TODO: maybe do not use a simple pointer (always use unique/shared ptr)
std::unique_ptr<ASTNode> AST::analyzeDeclarations(PTNode* node, Symbol::Type type) {

    for(unsigned i=0; i<(*node).children.size(); i++) {
        if((*node).children[i].type == PTNode::Type::Identifier) {
            // for declarator
            IdentifierToken identiferToken = static_cast<IdentifierToken&>((*node).children[i]);
            std::string identifier = identiferToken.getName();
            Symbol symbol = Symbol(type, (*node).children[i].source);
            symbolTable.declarations.insert(std::pair<std::string, Symbol>(identifier, symbol));
        }
    }
    return nullptr;
}

std::unique_ptr<ASTNode> AST::analyzeStatement(std::unique_ptr<PTNode> node) {
    return nullptr;
}
std::unique_ptr<ASTNode> AST::analyzeExpression(std::unique_ptr<PTNode> node) {
    return nullptr;
}
std::unique_ptr<ASTNode> AST::analyseFunction(std::unique_ptr<PTNode> node) {
    // TODO: where is the difference to analyzeParseTree? are multiple fun-ction definitions in a program allowed? oO
    return nullptr;
}