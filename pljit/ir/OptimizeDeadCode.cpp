#include "OptimizeDeadCode.hpp"
#include <iterator>
#include <vector>
//---------------------------------------------------------------------------
namespace pljit_ir {
//---------------------------------------------------------------------------
void OptimizeDeadCode::visit(pljit_ast::FunctionAST& functionAst) {
    std::vector<std::unique_ptr<pljit_ast::ASTNode>>::iterator it;
    for (it = functionAst.children.begin(); it < functionAst.children.end(); it++) {
        auto statement = static_cast<pljit_ast::StatementAST*>(it->get());
        if (statement->getType() == pljit_ast::ASTNode::Type::ReturnStatement) {
            functionAst.children.erase(it + 1, functionAst.children.end());
        }
    }
}
void OptimizeDeadCode::visit(pljit_ast::AssignmentAST&) {}
void OptimizeDeadCode::visit(pljit_ast::ReturnStatementAST&) {}
void OptimizeDeadCode::visit(pljit_ast::BinaryOperationAST&) {}
void OptimizeDeadCode::visit(pljit_ast::UnaryAST&) {}
void OptimizeDeadCode::visit(pljit_ast::LiteralAST&) {}
void OptimizeDeadCode::visit(pljit_ast::IdentifierAST&) {}
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------