#pragma once

#include "SourceCode.hpp"
#include <string>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
class SourceReference {
    SourceCode code;
public:
    SourceReference(unsigned lineNum, unsigned charPos, unsigned length, SourceCode& code);

    void printContext(std::string_view message) const;
    [[nodiscard]] std::string getText() const;
    [[nodiscard]] unsigned getLineNum() const;
    [[nodiscard]] unsigned getCharPos() const;
    [[nodiscard]] unsigned getLength() const;
private:
    unsigned lineNum;
    unsigned charPos;
    unsigned length;
};
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------