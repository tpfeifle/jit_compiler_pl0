#include "OptimizeDeadCode.hpp"
#include <iterator>
#include <vector>
//---------------------------------------------------------------------------
namespace pljit_ir {
//---------------------------------------------------------------------------
void OptimizeDeadCode::visit(pljit_ast::FunctionAST& functionAst) {
    std::vector<std::unique_ptr<pljit_ast::StatementAST>>::iterator it;
    for (it = functionAst.children.begin(); it < functionAst.children.end(); it++) {
        auto statement = static_cast<pljit_ast::StatementAST*>(it->get());
        if (statement->getType() == pljit_ast::ASTNode::Type::ReturnStatement) {
            functionAst.children.erase(it + 1, functionAst.children.end());
        }
    }
}

//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------