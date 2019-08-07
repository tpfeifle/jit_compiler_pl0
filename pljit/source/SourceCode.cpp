#include "SourceCode.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
int SourceCode::readCode() {
    std::ifstream in(filename.c_str()); // TODO use OS specific function instead
    if (!in) {
        // not valid input filename
        return -1;
    } else {
        std::string buffer;
        while (std::getline(in, buffer)) {
            if (!buffer.empty()) {
                codeLines.push_back(buffer);
            }
        }
        in.close();
        return 0;
    }
}

std::string SourceCode::getLine(unsigned index) {
    return codeLines.at(index);
}

char SourceCode::getCharacter(unsigned line, unsigned offset) {
    if (codeLines.size() > line && codeLines.at(line).size() > offset) {
        return codeLines.at(line).at(offset);
    } else {
        return -1;
    }
}

char SourceCode::getNextCharacter(unsigned line, unsigned offset) {
    if (codeLines.at(line).size() > offset) {
        return codeLines.at(line).at(offset + 1);
    } else if (codeLines.size() > line) {
        return '\n';
        // return codeLines.at(line).at(offset+1);
    } else {
        return -1;
    }
}

unsigned SourceCode::numberOfLines() {
    return codeLines.size();
}
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------