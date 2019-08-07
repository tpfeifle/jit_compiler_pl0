#include "OptimizeDeadCode.hpp"
#include <iterator>
#include <vector>
//---------------------------------------------------------------------------
namespace pljit::ir {
//---------------------------------------------------------------------------
using namespace std;
void OptimizeDeadCode::visitASTNode(ast::ASTNode* node) {
    switch (node->type) {
        case ast::ASTNode::Type::Statement:
            break;
        case ast::ASTNode::Type::Function: {
            auto function = static_cast<ast::FunctionAST*>(node);
            vector<unique_ptr<ast::ASTNode>>::iterator it;
            for (it = function->children.begin(); it < function->children.end(); it++) {
                auto statement = static_cast<ast::StatementAST*>(it->get());
                if (statement->statementType == ast::StatementAST::StatementType::Return) {
                    function->children.erase(it+1, function->children.end());
                }
            }
            break;
        }
        case ast::ASTNode::Type::Expression:
            break;
    }
}
//---------------------------------------------------------------------------
} // namespace ir
//---------------------------------------------------------------------------