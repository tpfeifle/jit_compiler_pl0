#include "EvalContext.hpp"
//---------------------------------------------------------------------------
namespace pljit_ir {
//---------------------------------------------------------------------------
EvalContext::EvalContext(std::unordered_map<std::string, std::pair<pljit_ast::Symbol, unsigned>>& symbolTable) {
    {
        for (const auto& variable: symbolTable) {
            if (variable.second.first.type == pljit_ast::Symbol::Const) {
                parameters.emplace(std::pair<std::string, int64_t>(variable.first, 0));
            }
        }
    }
}
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------