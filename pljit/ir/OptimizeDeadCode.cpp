#include "OptimizeDeadCode.hpp"
#include <iterator>
#include <vector>
//---------------------------------------------------------------------------
namespace pljit::ir {
//---------------------------------------------------------------------------
using namespace std;
void OptimizeDeadCode::visit(ast::FunctionAST& functionAst) {
    vector<unique_ptr<ast::ASTNode>>::iterator it;
    for (it = functionAst.children.begin(); it < functionAst.children.end(); it++) {
        auto statement = static_cast<ast::StatementAST*>(it->get());
        if (statement->getType() == ast::ASTNode::Type::ReturnStatement) {
            functionAst.children.erase(it + 1, functionAst.children.end());
        }
    }
}
void OptimizeDeadCode::visit(ast::AssignmentAST&) {}
void OptimizeDeadCode::visit(ast::ReturnStatementAST&) {}
void OptimizeDeadCode::visit(ast::BinaryOperationAST&) {}
void OptimizeDeadCode::visit(ast::UnaryAST&) {}
void OptimizeDeadCode::visit(ast::LiteralAST&) {}
void OptimizeDeadCode::visit(ast::IdentifierAST&) {}
//---------------------------------------------------------------------------
} // namespace ir
//---------------------------------------------------------------------------