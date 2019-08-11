#include "SourceCode.hpp"
#include <string>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
SourceCode::SourceCode(const std::string& codeText)
// Constructor (with the PL/0 code as parameter)
{
    std::istringstream stream(codeText);
    std::string line;
    while (std::getline(stream, line)) {
        codeLines.emplace_back(line + "\n");
    }
}
//---------------------------------------------------------------------------
std::string_view SourceCode::getLine(unsigned index) const
// Get a specific line of the code
{
    return codeLines.at(index);
}
//---------------------------------------------------------------------------
char SourceCode::getCharacter(unsigned line, unsigned offset) const
// Get a specific character from the code
{
    if (codeLines.size() > line && codeLines.at(line).size() > offset) {
        return codeLines.at(line).at(offset);
    } else {
        return -1;
    }
}
//---------------------------------------------------------------------------
unsigned SourceCode::numberOfLines() const
// Return the number of code lines
{
    return codeLines.size();
}
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------