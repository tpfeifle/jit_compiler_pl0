#pragma once

#include <vector>
#include <string>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
class SourceCode {
public:
    explicit SourceCode(std::vector<std::string> codeLines) : codeLines(std::move(codeLines)) {};
    [[nodiscard]] std::string getLine(unsigned index) const;
    [[nodiscard]] char getCharacter(unsigned line, unsigned offset) const;
    [[nodiscard]] unsigned numberOfLines() const;

private:
    std::vector<std::string> codeLines;
};
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------