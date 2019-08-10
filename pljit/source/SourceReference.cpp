#include "SourceReference.hpp"
#include "SourceCode.hpp"
#include <iostream>
#include <string_view>
//---------------------------------------------------------------------------
namespace pljit_source {
//---------------------------------------------------------------------------
// TODO: maybe use offset instead of lineNum, charPos
SourceReference::SourceReference(unsigned lineNum, unsigned charPos, unsigned length, SourceCode& code) : code(code), lineNum(lineNum),
                                                                                           charPos(charPos),
                                                                                           length(length) {
}
//---------------------------------------------------------------------------
void SourceReference::printContext(std::string_view message) const {
    std::cerr << lineNum << ":" << charPos << ":" << message << std::endl;
    std::cerr << code.getLine(lineNum);
    std::cerr << std::string(charPos, ' ') << '^' << std::string(length - 1, '~') << std::endl;
}
//---------------------------------------------------------------------------
std::string SourceReference::getText() const {
    return code.getLine(lineNum).substr(charPos, length); //TODO make performant: string_view?
}
//---------------------------------------------------------------------------
unsigned SourceReference::getLineNum() const {
    return lineNum;
}
//---------------------------------------------------------------------------
unsigned SourceReference::getCharPos() const {
    return charPos;
}
//---------------------------------------------------------------------------
unsigned SourceReference::getLength() const {
    return length;
}
//---------------------------------------------------------------------------
} // namespace pljit_source
//---------------------------------------------------------------------------