#include <pljit/parser/PTNode.hpp>
#include "OptimizeConstPropagation.hpp"
//---------------------------------------------------------------------------
namespace pljit_ir {
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
OptimizeConstPropagation::OptimizeConstPropagation(std::unordered_map<std::string, int> constValues) {
    this->constValues = std::move(constValues);
    // TODO initialize others as well
}
void OptimizeConstPropagation::visit(pljit_ast::FunctionAST& node) {
    for (auto&& child: node.children) {
        node_count++;
        child->accept(*this);
    }
}
void OptimizeConstPropagation::visit(pljit_ast::AssignmentAST& node) {
    node_count++;
    unsigned expressionNode = node_count;
    node.expression->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        constValues.insert({node.identifier->identifier, expressionMapping.at(expressionNode)});
        node.expression = make_unique<pljit_ast::LiteralAST>(expressionMapping.at(expressionNode));
    }
}
void OptimizeConstPropagation::visit(pljit_ast::ReturnStatementAST& node) {
    node_count++;
    unsigned expressionNode = node_count;
    node.expression->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        constValues.insert({"RETURN", expressionMapping.at(expressionNode)});
        node.expression = make_unique<pljit_ast::LiteralAST>(expressionMapping.at(expressionNode));
    }
}
void OptimizeConstPropagation::visit(pljit_ast::BinaryOperationAST& node) {
    unsigned currentNodeCount = node_count;
    node_count++;
    unsigned nodeLeft = node_count;
    node.left->accept(*this);
    node_count++;
    unsigned nodeRight = node_count;
    node.right->accept(*this);
    if (expressionMapping.find(nodeRight) != expressionMapping.end()) {
        node.right = make_unique<pljit_ast::LiteralAST>(expressionMapping.at(nodeRight));
    }
    if (expressionMapping.find(nodeLeft) != expressionMapping.end()) {
        node.left = make_unique<pljit_ast::LiteralAST>(expressionMapping.at(nodeLeft));
    }
    if (expressionMapping.find(nodeRight) != expressionMapping.end() &&
        expressionMapping.find(nodeLeft) != expressionMapping.end()) {
        switch (node.type) {
            case pljit_ast::BinaryOperationAST::Plus: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] + expressionMapping[nodeRight];
                break;
            }
            case pljit_ast::BinaryOperationAST::Minus: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] - expressionMapping[nodeRight];
                break;
            }
            case pljit_ast::BinaryOperationAST::Multiply: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] * expressionMapping[nodeRight];
                break;
            }
            case pljit_ast::BinaryOperationAST::Divide: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] / expressionMapping[nodeRight];
                break;
            }
        }
    }
}
void OptimizeConstPropagation::visit(pljit_ast::UnaryAST& node) {
    unsigned currentNode = node_count;
    node_count++;
    unsigned expressionNode = node_count;
    node.child->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        if (node.sign == pljit_ast::UnaryAST::Minus) {
            expressionMapping.insert({currentNode, -1 * expressionMapping[node_count]});
            node.child = make_unique<pljit_ast::LiteralAST>(-1 * expressionMapping[node_count]);
        } else {
            node.child = make_unique<pljit_ast::LiteralAST>(expressionMapping[node_count]);
        }
    }
}
void OptimizeConstPropagation::visit(pljit_ast::LiteralAST& node) {
    expressionMapping.insert({node_count, node.value});
}
void OptimizeConstPropagation::visit(pljit_ast::IdentifierAST& node) {
    if (constValues.find(node.identifier) != constValues.end()) {
        expressionMapping.insert({node_count, constValues.at(node.identifier)});
    }
}
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------