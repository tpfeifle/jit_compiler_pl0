#include <pljit/parser/PTNode.hpp>
#include "OptimizeConstPropagation.hpp"
//---------------------------------------------------------------------------
namespace pljit::ir {
using namespace std;
//---------------------------------------------------------------------------
OptimizeConstPropagation::OptimizeConstPropagation(std::unordered_map<std::string, int> constValues) {
    this->constValues = std::move(constValues);
    // TODO initialize others as well
}
void OptimizeConstPropagation::visit(ast::FunctionAST& node) {
    for (auto&& child: node.children) {
        node_count++;
        child->accept(*this);
    }
}
void OptimizeConstPropagation::visit(ast::AssignmentAST& node) {
    node_count++;
    unsigned expressionNode = node_count;
    node.expression->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        constValues.insert({node.identifier->identifier, expressionMapping.at(expressionNode)});
        node.expression = make_unique<ast::LiteralAST>(expressionMapping.at(expressionNode));
    }
}
void OptimizeConstPropagation::visit(ast::ReturnStatementAST& node) {
    node_count++;
    unsigned expressionNode = node_count;
    node.expression->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        constValues.insert({"RETURN", expressionMapping.at(expressionNode)});
        node.expression = make_unique<ast::LiteralAST>(expressionMapping.at(expressionNode));
    }
}
void OptimizeConstPropagation::visit(ast::BinaryOperationAST& node) {
    unsigned currentNodeCount = node_count;
    node_count++;
    unsigned nodeLeft = node_count;
    node.left->accept(*this);
    node_count++;
    unsigned nodeRight = node_count;
    node.right->accept(*this);
    //----
    if (expressionMapping.find(nodeRight) != expressionMapping.end()) {
        node.right = make_unique<ast::LiteralAST>(expressionMapping.at(nodeRight));
    }
    if (expressionMapping.find(nodeLeft) != expressionMapping.end()) {
        node.left = make_unique<ast::LiteralAST>(expressionMapping.at(nodeLeft));
    }
    //----
    if (expressionMapping.find(nodeRight) != expressionMapping.end() &&
        expressionMapping.find(nodeLeft) != expressionMapping.end()) {
        switch (node.type) {
            case ast::BinaryOperationAST::Plus: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] + expressionMapping[nodeRight];
                break;
            }
            case ast::BinaryOperationAST::Minus: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] - expressionMapping[nodeRight];
                break;
            }
            case ast::BinaryOperationAST::Multiply: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] * expressionMapping[nodeRight];
                break;
            }
            case ast::BinaryOperationAST::Divide: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] / expressionMapping[nodeRight];
                break;
            }
        }
    }
}
void OptimizeConstPropagation::visit(ast::UnaryAST& node) {
    unsigned currentNode = node_count;
    node_count++;
    unsigned expressionNode = node_count;
    node.child->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        if (node.sign == ast::UnaryAST::Minus) {
            expressionMapping.insert({currentNode, -1 * expressionMapping[node_count]});
            //----
            node.child = make_unique<ast::LiteralAST>(-1 * expressionMapping[node_count]);
            //----
        } else {
            //----
            node.child = make_unique<ast::LiteralAST>(expressionMapping[node_count]);
            //----
        }
    }
}
void OptimizeConstPropagation::visit(ast::LiteralAST& node) {
    expressionMapping.insert({node_count, node.value});
}
void OptimizeConstPropagation::visit(ast::IdentifierAST& node) {
    if (constValues.find(node.identifier) != constValues.end()) {
        expressionMapping.insert({node_count, constValues.at(node.identifier)});

        //auto& temp = dynamic_cast<ast::LiteralAST&>(constValues.at(node->identifier));
        //auto temp2 = temp.releaseInput();
        //node = move(temp2);
        //node = move(make_unique<ast::LiteralAST>(constValues.at(node->identifier)));

        //node = make_unique<ast::LiteralAST>(constValues.at(node->identifier));
    }
}
//---------------------------------------------------------------------------



/*
OptimizeConstPropagation2::OptimizeConstPropagation2(std::unordered_map<int, int> expressionMapping, std::unordered_map<std::string, int> constValues) {
    this->expressionMapping = std::move(expressionMapping);
    this->constValues = std::move(constValues);
}
void OptimizeConstPropagation2::visit(ast::FunctionAST& node) {
    for (auto&& child: node.children) {
        node_count++;
        child->accept(*this);
    }
}
void OptimizeConstPropagation2::visit(ast::AssignmentAST& node) {
    node_count++;
    unsigned expressionNode = node_count;
    node.expression->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        SourceReference foo = SourceReference(0, 0, 1, TODO);
        node = parser::LiteralPTNode(foo, constValues.at(node.identifier->identifier));
    }
}
void OptimizeConstPropagation2::visit(ast::ReturnStatementAST& node) {
    node_count++;
    unsigned expressionNode = node_count;
    node.expression->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        SourceReference foo = SourceReference(0, 0, 1, TODO);
        node = parser::LiteralPTNode(foo, expressionMapping.at(expressionNode));
    }
}
void OptimizeConstPropagation2::visit(ast::BinaryOperationAST& node) {
    unsigned currentNodeCount = node_count;
    node_count++;
    unsigned nodeLeft = node_count;
    node.left->accept(*this);
    node_count++;
    unsigned nodeRight = node_count;
    node.right->accept(*this);
    if (expressionMapping.find(nodeRight) != expressionMapping.end() &&
        expressionMapping.find(nodeLeft) != expressionMapping.end()) {
        switch (node.type) {
            case ast::BinaryOperationAST::Plus: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] + expressionMapping[nodeRight];
                break;
            }
            case ast::BinaryOperationAST::Minus: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] - expressionMapping[nodeRight];
                break;
            }
            case ast::BinaryOperationAST::Multiply: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] * expressionMapping[nodeRight];
                break;
            }
            case ast::BinaryOperationAST::Divide: {
                expressionMapping[currentNodeCount] =
                        expressionMapping[nodeLeft] / expressionMapping[nodeRight];
                break;
            }
        }
    }
}
void OptimizeConstPropagation2::visit(ast::UnaryAST& node) {
    unsigned currentNode = node_count;
    node_count++;
    unsigned expressionNode = node_count;
    node.child->accept(*this);
    if (expressionMapping.find(expressionNode) != expressionMapping.end()) {
        if (node.sign == ast::UnaryAST::Minus) {
            expressionMapping.insert({currentNode, -1 * expressionMapping[node_count]});
        }
    }
}
void OptimizeConstPropagation2::visit(ast::LiteralAST& node) {
    expressionMapping.insert({node_count, node.value});
}
void OptimizeConstPropagation2::visit(ast::IdentifierAST& node) {
    if (constValues.find(node.identifier) != constValues.end()) {
        expressionMapping.insert({node_count, constValues.at(node.identifier)});
    }
}*/
} // namespace pljit::ir
//---------------------------------------------------------------------------