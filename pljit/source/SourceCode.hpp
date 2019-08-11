#pragma once
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include <sstream>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
/// Centralized source code management to access the code fragments
class SourceCode {
public:
    /// Constructor (with the PL/0 code as parameter)
    explicit SourceCode(const std::string& codeText);
    /// get a specific line of the code
    [[nodiscard]] std::string_view getLine(unsigned index) const;
    /// get a specific character from the code
    [[nodiscard]] char getCharacter(unsigned line, unsigned offset) const;
    /// return the number of code lines
    [[nodiscard]] unsigned numberOfLines() const;

private:
    /// Store for all of the code
    std::vector<std::string> codeLines;
};
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------