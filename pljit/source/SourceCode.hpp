#pragma once

#include <vector>
#include <string>
#include <sstream>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
class SourceCode {
public:
    explicit SourceCode(const std::string& codeText);
    [[nodiscard]] std::string getLine(unsigned index) const;
    [[nodiscard]] char getCharacter(unsigned line, unsigned offset) const;
    [[nodiscard]] unsigned numberOfLines() const;

private:
    std::vector<std::string> codeLines;
};
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------