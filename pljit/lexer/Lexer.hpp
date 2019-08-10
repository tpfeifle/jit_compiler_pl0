#pragma once
#include <vector>
#include <optional>
#include "Token.hpp"
#include <memory>
//---------------------------------------------------------------------------
namespace pljit_lexer {
//---------------------------------------------------------------------------
class Lexer {
public:
    std::vector<Token> tokens;

    Lexer(pljit_source::SourceCode& code) : code(code), currentPos(0), currentLine(0) {}

    std::unique_ptr<Token> next();

    int determineCategory(Token& token, unsigned start, unsigned length); // assumes current line as line

    pljit_source::SourceCode& code;
private:
    int currentPos;
    unsigned currentLine;
};
//---------------------------------------------------------------------------
} // namespace pljit_lexer
//---------------------------------------------------------------------------