#include "ASTNode.hpp"
#include "DotASTVisitor.hpp"

//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
void DotASTVisitor::visit(FunctionAST& node) {
    std::cout << "AST" << node_count << "[label=\"Function\"];" << std::endl;
    unsigned currentNodeCount = node_count;
    for (auto&& child: node.children) {
        node_count++;
        std::cout << "AST" << currentNodeCount << " -> AST" << node_count << std::endl;
        child->accept(*this);
    }
}
//---------------------------------------------------------------------------
void DotASTVisitor::visit(ReturnStatementAST& node) {
    std::cout << "AST" << node_count << "[label=\"Return\"];" << std::endl;
    unsigned currentNodeCount = node_count;
    node_count++;
    std::cout << "AST" << currentNodeCount << " -> AST" << node_count << std::endl;
    node.expression->accept(*this);
}
//---------------------------------------------------------------------------
void DotASTVisitor::visit(AssignmentAST& node) {
    std::cout << "AST" << node_count << "[label=\"Assignment\"];" << std::endl;
    unsigned currentNodeCount = node_count;
    node_count++;
    std::cout << "AST" << currentNodeCount << " -> AST" << node_count << std::endl;
    node.identifier->accept(*this);
    node_count++;
    std::cout << "AST" << currentNodeCount << " -> AST" << node_count << std::endl;
    node.expression->accept(*this);
}
//---------------------------------------------------------------------------
void DotASTVisitor::visit(BinaryOperationAST& node) {
    unsigned currentNodeCount = node_count;
    switch (node.type) {
        case BinaryOperationAST::Plus:
            std::cout << "AST" << currentNodeCount << "[label=\"+\"];" << std::endl;
            break;
        case BinaryOperationAST::Minus:
            std::cout << "AST" << currentNodeCount << "[label=\"-\"];" << std::endl;
            break;
        case BinaryOperationAST::Multiply:
            std::cout << "AST" << currentNodeCount << "[label=\"*\"];" << std::endl;
            break;
        case BinaryOperationAST::Divide:
            std::cout << "AST" << currentNodeCount << "[label=\"/\"];" << std::endl;
            break;
    }
    node_count++;
    std::cout << "AST" << currentNodeCount << " -> " << "AST" << node_count << std::endl;
    node.left->accept(*this);

    node_count++;
    std::cout << "AST" << currentNodeCount << " -> " << "AST" << node_count << std::endl;
    node.right->accept(*this);
}
//---------------------------------------------------------------------------
void DotASTVisitor::visit(UnaryAST& node) {
    std::cout << "AST" << node_count << "[label=\" " << node.sign << " \"];" << std::endl;
    unsigned currentNodeCount = node_count;
    node_count++;
    std::cout << "AST" << currentNodeCount << " -> AST" << node_count << std::endl;
    node.child->accept(*this);
}
//---------------------------------------------------------------------------
void DotASTVisitor::visit(IdentifierAST& node) {
    std::cout << "AST" << node_count << "[label=\" " << node.identifier << " \"];" << std::endl;
}
//---------------------------------------------------------------------------
void DotASTVisitor::visit(LiteralAST& node) {
    std::cout << "AST" << node_count << "[label=\" " << node.getValue() << " \"];" << std::endl;
}
//---------------------------------------------------------------------------
} // namespace pljit_ast
//---------------------------------------------------------------------------