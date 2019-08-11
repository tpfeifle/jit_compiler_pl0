#pragma once
//---------------------------------------------------------------------------
#include "SourceCode.hpp"
#include <string>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
/// Reference to a location or range in the original source code
class SourceReference {
public:
    /// Constructor taking the location of the reference and a reference of the source code
    SourceReference(unsigned lineNum, unsigned charPos, unsigned length, SourceCode& code);

    /// Output the context of the source reference in the source code
    void printContext(std::string_view message) const;
    /// Get the specified location from the source code
    [[nodiscard]] std::string_view getText() const;
    /// Get the lineNum member variable
    [[nodiscard]] unsigned getLineNum() const;
    /// Get the charPos member variable
    [[nodiscard]] unsigned getCharPos() const;
    /// Get the length member variable
    [[nodiscard]] unsigned getLength() const;

private:
    SourceCode& code;
    unsigned lineNum;
    unsigned charPos;
    unsigned length;
};
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------