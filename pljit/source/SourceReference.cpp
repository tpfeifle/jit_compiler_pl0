#include "SourceReference.hpp"
#include "SourceCode.hpp"

#include <iostream>
#include <string_view>

// TODO: maybe use offset instead of lineNum, charPos
SourceReference::SourceReference(int lineNum, int charPos, int length, SourceCode& code) : code(code), lineNum(lineNum),
                                                                                           charPos(charPos),
                                                                                           length(length) {
    /*assert(length > 0);
    assert(charPos >= 0);
    assert(lineNum >= 0);*/
    // TODO add again
}

void SourceReference::printContext(const std::string& message) {
    std::cerr << lineNum << ":" << charPos << ":" << message << std::endl;
    std::cerr << code.getLine(lineNum);
    std::cerr << std::string(charPos, ' ') << '^' << std::string(length-1, '~') << std::endl;
}

std::string SourceReference::getText() {
    std::string line = code.getLine(lineNum); //TODO make performant: string_view?
    return line.substr(charPos, length);
}