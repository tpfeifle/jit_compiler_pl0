#pragma once
//---------------------------------------------------------------------------
#include <unordered_map>
#include <pljit/ast/Symbol.hpp>
//---------------------------------------------------------------------------
namespace pljit_ir {
//---------------------------------------------------------------------------
/// Stores all variables used in the program (Evaluation context)
class EvalContext {
public:
    /// Constructor: initializes variables
    explicit EvalContext(std::unordered_map<std::string, std::pair<pljit_ast::Symbol, unsigned>> &symbolTable);

    /// Mapping from variables to values
    std::unordered_map<std::string, int64_t> parameters {};

    /// ErrorCode during the evaluation
    unsigned errorCode = 0; // TODO
};
//---------------------------------------------------------------------------
} // namespace pljit_ir
//---------------------------------------------------------------------------