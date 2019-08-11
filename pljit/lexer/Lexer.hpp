#pragma once
//---------------------------------------------------------------------------
#include <vector>
#include <optional>
#include "Token.hpp"
#include <memory>
//---------------------------------------------------------------------------
namespace pljit_lexer {
//---------------------------------------------------------------------------
/// Transforms the source code into a stream of Tokens
class Lexer {
public:
    /// Constructor
    Lexer(pljit_source::SourceCode code) : code(std::move(code)), currentPos(0), currentLine(0) {}

    /// Provides the next Token from the source code
    std::unique_ptr<Token> next();

    /// Determines the category of the token
    std::unique_ptr<Token> determineNextToken(unsigned start, unsigned length);

    /// Reference to the source code
    pljit_source::SourceCode code;
private:
    /// The current char- and line-position of the lexer in the code
    int currentPos;
    unsigned currentLine;
};
//---------------------------------------------------------------------------
} // namespace pljit_lexer
//---------------------------------------------------------------------------