#include "SourceCode.hpp"
#include <string>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
std::string SourceCode::getLine(unsigned index) {
    return codeLines.at(index);
}

char SourceCode::getCharacter(unsigned line, unsigned offset) {
    if (codeLines.size() > line && codeLines.at(line).size() > offset) {
        return codeLines.at(line).at(offset);
    } else {
        return -1;
    }
}

unsigned SourceCode::numberOfLines() {
    return codeLines.size();
}
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------