#include "OptimizeConstPropagation.hpp"
//---------------------------------------------------------------------------
namespace pljit::ir {
using namespace std;
//---------------------------------------------------------------------------
void OptimizeConstPropagation::visitASTNode(pljit::ast::ASTNode* node) {
    /*switch (node->type) {
        case ast::ASTNode::Type::Statement: {
            auto statement = static_cast<ast::StatementAST*>(node);
            switch(statement->statementType) {
                case ast::StatementAST::StatementType::Return: {
                    auto statementReturn = static_cast<ast::ReturnStatementAST*>(node);
                    visitASTNode(statementReturn->expression.get());
                    break;
                }
                case ast::StatementAST::StatementType::Assignment: {
                    auto assignment = static_cast<ast::AssignmentAST*>(node);
                    visitASTNode(assignment->expression.get());
                    break;
                }
                case ast::StatementAST::StatementType::ConstAssignment: {
                    break;
                }
                case ast::StatementAST::StatementType::ConstDecl: {
                    auto constDecl = static_cast<ast::ConstDeclAST*>(node);
                    for(auto&&child: constDecl->children) {
                        visitASTNode(child.get());
                    }
                    break;
                }
            }
            break;
        }
        case ast::ASTNode::Type::Function: {
            auto function = static_cast<ast::FunctionAST*>(node);
            for(auto&& child:function->children) {
                visitASTNode(child.get());
            }
            break;
        }
        case ast::ASTNode::Type::Expression:
            auto expression = static_cast<ast::ExpressionAST*>(node);
            switch(expression->type) {
                case ast::ExpressionAST::Type::Binary: {
                    break;
                }
                case ast::ExpressionAST::Type::Unary: {
                    break;
                }
                case ast::ExpressionAST::Type::Literal: {
                    break;
                }
                case ast::ExpressionAST::Type::Identifier: {
                    auto identifier = static_cast<ast::IdentifierAST*>(expression);
                    if(constValues.find(identifier->identifier) != constValues.end()) {

                    }
                    break;
                }
            }
            break;
    }*/
}
//---------------------------------------------------------------------------
} // namespace pljit::ir
//---------------------------------------------------------------------------