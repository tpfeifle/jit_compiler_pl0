#pragma once

#include "SourceCode.hpp"
#include <string>

using std::string;

class SourceReference {
private:
    SourceCode code;
public:
    SourceReference(int lineNum, int charPos, int length, SourceCode& code);

    void printContext(const std::string& message);
    std::string getText();
    int lineNum;
    int charPos;
    int length;
    std::string toString() {
        return std::to_string(lineNum) + std::to_string(charPos) + std::to_string(length);
    }
};