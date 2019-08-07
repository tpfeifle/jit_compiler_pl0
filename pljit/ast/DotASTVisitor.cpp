#include "ASTNode.hpp"
#include "DotASTVisitor.hpp"

//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
namespace ast {
//---------------------------------------------------------------------------
void DotASTVisitor::visitASTNode(ASTNode* node) {
    switch (node->getType()) {
        case ASTNode::Type::Expression: {
            auto foo = static_cast<ExpressionAST*>(node);
            switch (foo->type) {
                case ExpressionAST::Type::Binary: {
                    auto binary = static_cast<BinaryOperationAST*>(node);
                    switch (binary->type) {
                        case BinaryOperationAST::Plus:
                            std::cout << "AST" << binary->_id << "[label=\"+\"];" << std::endl;
                            break;
                        case BinaryOperationAST::Minus:
                            std::cout << "AST" << binary->_id << "[label=\"-\"];" << std::endl;
                            break;
                        case BinaryOperationAST::Multiply:
                            std::cout << "AST" << binary->_id << "[label=\"*\"];" << std::endl;
                            break;
                        case BinaryOperationAST::Divide:
                            std::cout << "AST" << binary->_id << "[label=\"/\"];" << std::endl;
                            break;
                    }

                    visitASTNode(binary->left.get());
                    std::cout << "AST" << binary->_id << " -> " << "AST" << binary->left->_id << std::endl;

                    std::cout << "AST" << binary->_id << " -> " << "AST" << binary->right->_id << std::endl;
                    visitASTNode(binary->right.get());
                }
                    break;
                case ExpressionAST::Type::Unary: {
                    auto unary = static_cast<UnaryAST*>(node);
                    std::cout << "Unary" << unary->sign << std::endl;
                    visitASTNode(unary->child.get());
                    break;
                }
                case ExpressionAST::Type::Identifier: {
                    auto identifier = static_cast<IdentifierAST*>(node);
                    std::cout << "AST" << identifier->_id << "[label=\"" << identifier->identifier << "\"];"
                              << std::endl;
                    break;
                }
                case ExpressionAST::Type::Literal: {
                    auto literal = static_cast<LiteralAST*>(node);
                    std::cout << "AST" << literal->_id << "[label=\"" << literal->value << "\"];" << std::endl;
                    break;
                }
            }
            break;
        }
        case ASTNode::Type::Statement: {
            auto fooSt = static_cast<StatementAST*>(node);
            switch (fooSt->statementType) {
                case StatementAST::StatementType::ConstDecl: {
                    auto foo = static_cast<ConstDeclAST*>(node);
                    for (unsigned i = 0; i < foo->children.size(); i++) {
                        std::cout << "AST" << foo->_id << " -> " << "AST" << foo->children[i]->_id << std::endl;
                        visitASTNode(foo->children[i].get());
                    }
                    break;
                }
                case StatementAST::StatementType::ConstAssignment: {
                    auto foo = static_cast<ConstAssignmentAST*>(node);
                    std::cout << "AST" << foo->_id << "[label=\"ConstAssignment\"];" << std::endl;
                    std::cout << "AST" << foo->value->_id << "[label=\"Value\"];" << std::endl;
                    std::cout << "AST" << foo->identifier->_id << "[label=\"Identifier\"];" << std::endl;

                    std::cout << "AST" << foo->_id << " -> " << "AST" << foo->value->_id << std::endl;
                    std::cout << "AST" << foo->_id << " -> " << "AST" << foo->identifier->_id << std::endl;
                    break;
                }
                case StatementAST::StatementType::Assignment: {
                    auto foo = static_cast<AssignmentAST*>(node);
                    std::cout << "AST" << foo->_id << "[label=\"Assignment\"];" << std::endl;
                    std::cout << "AST" << foo->identifier->_id << "[label=\"Identifier\"];" << std::endl;
                    std::cout << "AST" << foo->_id << " -> " << "AST" << foo->identifier->_id << std::endl;

                    std::cout << "AST" << foo->_id << " -> " << "AST" << foo->expression->_id << std::endl;
                    visitASTNode(foo->expression.get());
                    break;
                }
                case StatementAST::StatementType::Return: {
                    auto foo = static_cast<ReturnStatementAST*>(node);
                    std::cout << "AST" << foo->_id << "[label=\"Return\"];" << std::endl;
                    std::cout << "AST" << foo->_id << " -> " << "AST" << foo->expression->_id << std::endl;
                    visitASTNode(foo->expression.get());
                    break;
                }
            }
            break;
        }
        case ASTNode::Type::Function: {
            auto foo = static_cast<FunctionAST*>(node);
            std::cout << "AST" << foo->_id << "[label=\"Function\"];" << std::endl;
            for (unsigned i = 0; i < foo->children.size(); i++) {
                std::cout << "AST" << foo->_id << " -> " << "AST" << foo->children[i].get()->_id << std::endl;
                visitASTNode(foo->children[i].get());
            }
            break;
        }
        default:
            std::cerr << "why are you even here. fix this now!" << std::endl;
    }
}
//---------------------------------------------------------------------------
} //namespace ast
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------