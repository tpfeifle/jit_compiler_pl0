#include "Lexer.hpp"
#include <string>
//---------------------------------------------------------------------------
namespace pljit_lexer {
//---------------------------------------------------------------------------
std::unique_ptr<Token> Lexer::next()
// Provides the next Token from the source code
{
    unsigned beginningOfToken;
    while (currentLine < code.numberOfLines()) {
        int i = currentPos;
        while (code.getCharacter(currentLine, i) != -1) {
            if (!isspace(code.getCharacter(currentLine, i))) {
                if (!determineNextToken(i, 1)) { //invalid character
                    pljit_source::SourceReference source = pljit_source::SourceReference(currentLine, currentPos, 1,
                                                                                         code);
                    source.printContext("Unexpected character");
                    return nullptr;
                }
                beginningOfToken = i;

                // greedy add
                while (code.getCharacter(currentLine, i) != -1) {
                    if (isspace(code.getCharacter(currentLine, i)) ||
                        !determineNextToken(beginningOfToken, i + 1 - beginningOfToken)) {
                        currentPos = i;
                        std::unique_ptr<Token> res = determineNextToken(beginningOfToken, i - beginningOfToken);
                        return res;
                    }
                    i++;
                }
            }
            i++;
        }
        if (currentLine + 1 == code.numberOfLines()) {
            break;
        }
        currentLine++;
        currentPos = 0;
    }
    // End of the source-code is reached
    std::unique_ptr<Token> endToken = std::make_unique<Token>(
            pljit_source::SourceReference(currentLine, currentPos, 1, code),
            Token::Type::Invalid);
    return endToken;
}
//---------------------------------------------------------------------------
std::unique_ptr<Token> Lexer::determineNextToken(unsigned start, unsigned length)
// Determines the category of the token
{
    char firstChar = code.getCharacter(currentLine, start); //read the character from the source-code
    pljit_source::SourceReference source = pljit_source::SourceReference(currentLine, start, length, code);
    if (firstChar == '+' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Plus);
    } else if (firstChar == '-' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Minus);
    } else if (firstChar == '=' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Equal);
    } else if (firstChar == '*' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Multiply);
    } else if (firstChar == '/' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Divide);
    } else if (firstChar == '.' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Final);
    } else if (firstChar == ':' &&
               (length == 1 || (length == 2 && code.getCharacter(currentLine, start + length - 1) == '='))) {
        return std::make_unique<Token>(source, Token::Type::Assignment);
    } else if (firstChar == ',' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Colon);
    } else if (firstChar == ';' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Semicolon);
    } else if (firstChar == '(' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Left_Bracket);
    } else if (firstChar == ')' && length == 1) {
        return std::make_unique<Token>(source, Token::Type::Right_Bracket);
    } else if (isdigit(firstChar)) {
        // Check if it contains only digits --> is a literal
        for (unsigned i = 0; i < length; i++) {
            if (!isdigit(code.getCharacter(currentLine, start + i))) {
                return nullptr;
            }
        }
        return std::make_unique<Token>(source, Token::Type::Literal);
    } else {
        // if not only letters there is a problem
        for (unsigned i = 0; i < length; i++) {
            if (!isalpha(code.getCharacter(currentLine, start + i))) {
                return nullptr;
            }
        }

        // Check if the token matches any keyword, else it is an identifier
        std::string_view tokenAsString = code.getLine(currentLine).substr(start, length);
        if (tokenAsString == "PARAM") {
            return std::make_unique<Token>(source, Token::Type::Param);
        } else if (tokenAsString == "VAR") {
            return std::make_unique<Token>(source, Token::Type::Var);
        } else if (tokenAsString == "CONST") {
            return std::make_unique<Token>(source, Token::Type::Const);
        } else if (tokenAsString == "BEGIN") {
            return std::make_unique<Token>(source, Token::Type::Begin);
        } else if (tokenAsString == "END") {
            return std::make_unique<Token>(source, Token::Type::End);
        } else if (tokenAsString == "RETURN") {
            return std::make_unique<Token>(source, Token::Type::Return);
        } else {
            return std::make_unique<Token>(source, Token::Type::Identifier);
        }
    }
    return nullptr;
}
//---------------------------------------------------------------------------
} // namespace pljit_lexer
//---------------------------------------------------------------------------