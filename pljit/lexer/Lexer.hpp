#pragma once
#include <vector>
#include <optional>
#include "Token.hpp"
#include <memory>

class Lexer {
public:
    std::vector<Token> tokens;

    Lexer(SourceCode& code) : code(code), currentPos(0), currentLine(0) {}

    //void scanCode();

    std::unique_ptr<Token> next();

    std::unique_ptr<Token> nextLookahead();

    int determineCategory(Token &token, unsigned start, unsigned length); // assumes current line as line

    SourceCode& code;
private:
    int currentPos;
    unsigned currentLine;
};