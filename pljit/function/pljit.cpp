#include <pljit/lexer/Lexer.hpp>
#include "pljit.hpp"

//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
FunctionHandle Pljit::registerFunction(const std::string& codeText) {
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    functions.emplace_back(std::make_unique<PljitFunction>(code));
    return FunctionHandle(functions.back().get());
}
} // namespace pljit_function
//---------------------------------------------------------------------------