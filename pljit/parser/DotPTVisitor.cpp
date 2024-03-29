#include "DotPTVisitor.hpp"
#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
std::string DotPTVisitor::nodeTypeToString(PTNode::Type type)
// Returns the parse-tree node-types as a string
{
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
void DotPTVisitor::visit(IdentifierPTNode& node)
// Visitor method outputting the identifier node in the dot-format
{
    std::cout << "Identifier" << node_count << "[label=\"" << node.getName() << "\"];" << std::endl;
    node_count++;
}
//---------------------------------------------------------------------------
void DotPTVisitor::visit(LiteralPTNode& node)
// Visitor method outputting a literal node in the dot-format
{
    std::cout << "Literal" << node_count << "[label=\"" << node.getValue() << "\"];" << std::endl;
    node_count++;
}
//---------------------------------------------------------------------------
void DotPTVisitor::visit(GenericTokenPTNode& node)
// Visitor method outputting a generic token in the dot-format
{
    std::cout << "GenericToken" << node_count << "[label=\"" << node.source.getText() << "\"];" << std::endl;
    node_count++;
}
//---------------------------------------------------------------------------
void DotPTVisitor::visit(NonTerminalPTNode& node)
// Visitor method outputting a non-terminal in the dot-format and visiting all of its children
{
    std::string label = nodeTypeToString(node.getType());
    unsigned thisNodeId = node_count;
    std::cout << label << thisNodeId << "[label=\"" << label << "\"];" << std::endl;
    for (auto&& child: node.children) {
        node_count++;
        std::cout << label << thisNodeId << " -> "
                  << nodeTypeToString(child->getType())
                  << node_count << std::endl;
        child->accept(*this);
    }
}
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------
