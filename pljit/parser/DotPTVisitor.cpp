#include "DotPTVisitor.hpp"
#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit::parser {
//---------------------------------------------------------------------------
std::string DotPTVisitor::nodeTypeToString(PTNode::Type type) {
    std::string label;
    switch (type) {
        case PTNode::Type::Statement:
            label = "Statement";
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
        case PTNode::Type::GenericToken:
            label = "GenericToken";
            break;
        case PTNode::Type::Identifier:
            label = "Identifier";
            break;
        case PTNode::Type::Literal:
            label = "Literal";
            break;
        default:
            label = "FunctionDefinition";
            break;
    }
    return label;
}
//---------------------------------------------------------------------------
void DotPTVisitor::visit(IdentifierPTNode& node) {
    std::cout << "Identifier" << node_count << "[label=\"" << node.getName() << "\"];" << std::endl;
    node_count++;
}
void DotPTVisitor::visit(LiteralPTNode& node) {
    std::cout << "Literal" << node_count << "[label=\""<< node.getValue() << "\"];" << std::endl;
    node_count++;
}
void DotPTVisitor::visit(GenericTokenPTNode& node) {
    std::cout << "GenericToken" << node_count << "[label=\"" << node.source.getText() << "\"];" << std::endl;
    node_count++;
}
void DotPTVisitor::visit(NonTerminalPTNode& node) {
    std::string label = nodeTypeToString(node.getType());
    unsigned thisNodeId = node_count;
    std::cout << label << thisNodeId << "[label=\""<< label << "\"];" << std::endl;
    for (auto&& child: node.children) {
        node_count++;
        std::cout << label << thisNodeId << " -> "
                  << nodeTypeToString(child->getType())
                  << node_count << std::endl;
        child->accept(*this);
    }
}
//---------------------------------------------------------------------------
} // namespace pljit::parser
//---------------------------------------------------------------------------
