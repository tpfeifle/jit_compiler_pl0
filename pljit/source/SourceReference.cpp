#include "SourceReference.hpp"
#include "SourceCode.hpp"
#include <iostream>
#include <string_view>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
SourceReference::SourceReference(unsigned lineNum, unsigned charPos,
                                 unsigned length, SourceCode& code) : code(code), lineNum(lineNum),
                                                                      charPos(charPos), length(length)
// Constructor
{}
//---------------------------------------------------------------------------
void SourceReference::printContext(std::string_view message) const
// Output the context of the source reference in the source code
{
    std::cerr << lineNum << ":" << charPos << ":" << message << std::endl;
    std::cerr << code.getLine(lineNum);
    std::cerr << std::string(charPos, ' ') << '^' << std::string(length - 1, '~') << std::endl;
}
//---------------------------------------------------------------------------
std::string_view SourceReference::getText() const
// Get the specified location from the source code
{
    return code.getLine(lineNum).substr(charPos, length);
}
//---------------------------------------------------------------------------
unsigned SourceReference::getLineNum() const
// Get the lineNum member variable
{
    return lineNum;
}
//---------------------------------------------------------------------------
unsigned SourceReference::getCharPos() const
// Get the charPos member variable
{
    return charPos;
}
//---------------------------------------------------------------------------
unsigned SourceReference::getLength() const
// Get the length member variable
{
    return length;
}
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------