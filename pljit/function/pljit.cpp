#include <pljit/lexer/Lexer.hpp>
#include "pljit.hpp"

//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
FunctionHandle Pljit::registerFunction(std::vector<std::string> codeText) {
    pljit_source::SourceCode code = pljit_source::SourceCode(std::move(codeText));
    functions.emplace_back(std::make_unique<PljitFunction>(code));
    return FunctionHandle(functions.back().get());
}
} // namespace pljit_function
//---------------------------------------------------------------------------