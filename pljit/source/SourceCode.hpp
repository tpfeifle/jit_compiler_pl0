#pragma once

#include <vector>
#include <string>
//---------------------------------------------------------------------------
namespace pljit::source {
//---------------------------------------------------------------------------
class SourceCode {
public:
    explicit SourceCode(std::string filename) : filename(std::move(filename)) {};
    explicit SourceCode(std::vector<std::string> codeLines) : codeLines(std::move(codeLines)) {}; // Useful for testing

    std::string getLine(unsigned index);
    char getCharacter(unsigned line, unsigned offset);
    char getNextCharacter(unsigned line, unsigned offset);

    int readCode();

    unsigned numberOfLines();

private:
    std::vector<std::string> codeLines;
    std::string filename;
};
//---------------------------------------------------------------------------
} // namespace pljit::source
//---------------------------------------------------------------------------