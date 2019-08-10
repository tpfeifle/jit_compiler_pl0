#include "Lexer.hpp"
#include <string>
#include <optional>
#include <string_view>
#include <iostream>
//---------------------------------------------------------------------------
namespace pljit_lexer {
//---------------------------------------------------------------------------
std::unique_ptr<Token> Lexer::next() {
    while (currentLine < code.numberOfLines()) {
        unsigned i = currentPos;
        unsigned beginningOfToken = 0;
        while (code.getCharacter(currentLine, i) != -1) {
            if (!isspace(code.getCharacter(currentLine, i))) {
                Token token = Token(pljit_source::SourceReference(0, 0, 1, code),
                                              Token::Type::Plus); // TODO: this feels stupid to use as a placeholder

                if (determineCategory(token, i, 1) == -1) {
                    //invalid character -->
                    token.source.printContext("Unexpected character");
                    return nullptr;
                    // break;
                }
                beginningOfToken = i;

                // greedy add
                while (code.getCharacter(currentLine, i)) {
                    if (isspace(code.getCharacter(currentLine, i)) ||
                        determineCategory(token, beginningOfToken, i + 1 - beginningOfToken) == -1) {
                        determineCategory(token, beginningOfToken, i - beginningOfToken);
                        tokens.emplace_back(token);
                        currentPos = i;
                        return std::make_unique<Token>(tokens.back());
                    }
                    i++;
                }
                break;
            }
            i++;
        }
        currentLine++;
        currentPos = 0;
    }
    // std::cout << "why am I even here -.- (Lexer)" << std::endl;
    return nullptr;
}
//---------------------------------------------------------------------------
int Lexer::determineCategory(Token& token, unsigned start, unsigned length) {
    char firstChar = code.getCharacter(currentLine, start);
    pljit_source::SourceReference source = pljit_source::SourceReference(currentLine, start, length, code);
    if (firstChar == '+' && length == 1) {
        token = Token(source, Token::Type::Plus);
    } else if (firstChar == '-' && length == 1) {
        token = Token(source, Token::Type::Minus);
    } else if (firstChar == '=' && length == 1) {
        token = Token(source, Token::Type::Equal);
    } else if (firstChar == '*' && length == 1) {
        token = Token(source, Token::Type::Multiply);
    } else if (firstChar == '/' && length == 1) {
        token = Token(source, Token::Type::Divide);
    } else if (firstChar == '.' && length == 1) {
        token = Token(source, Token::Type::Final);
    } else if (firstChar == ':' &&
               (length == 1 || (length == 2 && code.getCharacter(currentLine, start + length - 1) == '='))) {
        token = Token(source, Token::Type::Assignment);
    } else if (firstChar == ',' && length == 1) {
        token = Token(source, Token::Type::Colon);
    } else if (firstChar == ';' && length == 1) {
        token = Token(source, Token::Type::Semicolon);
    } else if (firstChar == '(' && length == 1) {
        token = Token(source, Token::Type::Left_Bracket);
    } else if (firstChar == ')' && length == 1) {
        token = Token(source, Token::Type::Right_Bracket);
    } else if (isdigit(firstChar)) {
        for (unsigned i = 0; i < length; i++) {
            if (!isdigit(code.getCharacter(currentLine, start + i))) {
                token = Token(source, Token::Type::NOT_USED);
                return -1;
            }
        }
        token = Token(source, Token::Type::Literal);
    } else {
        for (unsigned i = 0; i < length; i++) {
            if (!isalpha(code.getCharacter(currentLine, start + i))) {
                token = Token(source, Token::Type::NOT_USED);
                return -1;
            }
        }

        std::string tokenAsString = std::string(code.getLine(currentLine)).substr(start,
                                                                                  length); // TODO maybe use string_view instead

        if (tokenAsString == "PARAM") {
            token = Token(source, Token::Type::Param);
        } else if (tokenAsString == "VAR") {
            token = Token(source, Token::Type::Var);
        } else if (tokenAsString == "CONST") {
            token = Token(source, Token::Type::Const);
        } else if (tokenAsString == "BEGIN") {
            token = Token(source, Token::Type::Begin);
        } else if (tokenAsString == "END") {
            token = Token(source, Token::Type::End);
        } else if (tokenAsString == "RETURN") {
            token = Token(source, Token::Type::Return);
        } else {
            token = Token(source, Token::Type::Identifier);
        }
    }
    return 1;
}
//---------------------------------------------------------------------------
} // namespace pljit_lexer
//---------------------------------------------------------------------------