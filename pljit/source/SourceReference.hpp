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

    void printContext(const std::string& message);
    std::string getText();
    unsigned lineNum;
    unsigned charPos;
    unsigned length;
    std::string toString() {
        return std::to_string(lineNum) + std::to_string(charPos) + std::to_string(length);
    }
};
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------