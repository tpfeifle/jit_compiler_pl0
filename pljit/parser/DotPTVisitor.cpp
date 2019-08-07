#include "DotPTVisitor.hpp"
#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
std::string DotPTVisitor::nodeTypeToString(PTNode::Type type) {
    std::string label;
    switch (type) {
        case PTNode::Type::Statement:
            label = "Statement";
            break;
        case PTNode::Type::Identifier:
            label = "Identifier";
            break;
        case PTNode::Type::Literal:
            label = "Literal";
            break;
        case PTNode::Type::GenericToken:
            label = "GenericToken";
            break;
        case PTNode::Type::AdditiveExpr:
            label = "AdditiveExpr";
            break;
        case PTNode::Type::MultiplicativeExpr:
            label = "MultiplicativeExpr";
            break;
        case PTNode::Type::AssignmentExpr:
            label = "AssignmentExpr";
            break;
        case PTNode::Type::UnaryExpr:
            label = "UnaryExpr";
            break;
        case PTNode::Type::PrimaryExpr:
            label = "PrimaryExpr";
            break;
        case PTNode::Type::StatementList:
            label = "StatementList";
            break;
        case PTNode::Type::CompoundStatement:
            label = "CompoundStatement";
            break;
        case PTNode::Type::InitDeclarator:
            label = "InitDeclarator";
            break;
        case PTNode::Type::InitDeclaratorList:
            label = "InitDeclaratorList";
            break;
        case PTNode::Type::DeclaratorList:
            label = "DeclaratorList";
            break;
        case PTNode::Type::ConstDeclaration:
            label = "ConstDeclaration";
            break;
        case PTNode::Type::VarDeclaration:
            label = "VarDeclaration";
            break;
        case PTNode::Type::ParamDeclaration:
            label = "ParamDeclaration";
            break;
        default:
            label = "FunctionDefinition";
            break;
    }
    return label;
}
//---------------------------------------------------------------------------
void DotPTVisitor::visitPTNode(PTNode* node) {
    std::string label = nodeTypeToString(node->getType());

    std::cout << label << node->source.toString() << "[label=\"" << label << "\"];" << std::endl;
    if (node->getType() != PTNode::Type::GenericToken && node->getType() != PTNode::Type::Literal &&
        node->getType() != PTNode::Type::Identifier) {
        NonTerminalPTNode* nodeTyped = static_cast<NonTerminalPTNode*>(node);
        for (unsigned i = 0; i < nodeTyped->children.size(); i++) {
            std::cout << label << node->source.toString() << " -> "
                      << nodeTypeToString(nodeTyped->children[i]->getType())
                      << nodeTyped->children[i]->source.toString() << std::endl;
            visitPTNode(nodeTyped->children[i].get());
        }
    }
}
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------
