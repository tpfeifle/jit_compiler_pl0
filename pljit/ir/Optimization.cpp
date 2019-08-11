#include "Optimization.hpp"
//---------------------------------------------------------------------------
namespace pljit_ir {
//---------------------------------------------------------------------------
/// Defaults for the different visitor methods
void Optimization::optimizeAST(pljit_ast::FunctionAST& node) {
    visit(node);
}
void Optimization::visit(pljit_ast::FunctionAST& node) {
    for (auto&& child: node.children) {
        child->accept(*this);
    }
}
void Optimization::visit(pljit_ast::LiteralAST&) {}
void Optimization::visit(pljit_ast::UnaryAST&) {}
void Optimization::visit(pljit_ast::IdentifierAST&) {}
void Optimization::visit(pljit_ast::AssignmentAST& node) {
    node.expression->accept(*this);
    node.identifier->accept(*this);
}
void Optimization::visit(pljit_ast::ReturnStatementAST& node) {
    node.expression->accept(*this);
}
void Optimization::visit(pljit_ast::BinaryOperationAST& node) {
    node.left->accept(*this);
    node.right->accept(*this);
}
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------