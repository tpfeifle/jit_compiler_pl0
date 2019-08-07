#include "Lexer.hpp"
#include <string>
#include <optional>
#include <string_view>
#include <iostream>
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
std::unique_ptr<Token> Lexer::next() {
    while (currentLine < code.numberOfLines()) {
        int i = currentPos;
        int beginningOfToken = 0;
        while (code.getCharacter(currentLine, i) != -1) {
            if (!isspace(code.getCharacter(currentLine, i))) {
                Token token = Token(SourceReference(0, 0, 1, code), Token::Category::OPERATOR,
                                    Token::Type::PLUS); // TODO: this feels stupid to use as a placeholder

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
    std::cout << "why am I even here -.- (Lexer)" << std::endl;
    return nullptr;
}

/**
 * return next token, but do not increment the currentLine or currentPos --> used for repeats to look ahead
 * @return
 */
//---------------------------------------------------------------------------
std::unique_ptr<Token> Lexer::nextLookahead() {
    unsigned backupCurrentLine = currentLine;
    unsigned backupCurrentPos = currentPos;
    std::unique_ptr<Token> token = this->next();
    currentLine = backupCurrentLine;
    currentPos = backupCurrentPos;
    if (token) {
        tokens.pop_back();
    }
    return token;
}
//---------------------------------------------------------------------------
int Lexer::determineCategory(Token& token, unsigned start, unsigned length) {
    char firstChar = code.getCharacter(currentLine, start);
    SourceReference source = SourceReference(currentLine, start, length, code);
    if (firstChar == '+' && length == 1) {
        token = Token(source, Token::Category::OPERATOR, Token::Type::PLUS);
    } else if (firstChar == '-' && length == 1) {
        token = Token(source, Token::Category::OPERATOR, Token::Type::MINUS);
    } else if (firstChar == '=' && length == 1) {
        token = Token(source, Token::Category::OPERATOR, Token::Type::EQUAL);
    } else if (firstChar == '*' && length == 1) {
        token = Token(source, Token::Category::OPERATOR, Token::Type::MULTIPLY);
    } else if (firstChar == '/' && length == 1) {
        token = Token(source, Token::Category::OPERATOR, Token::Type::DIVIDE);
    } else if (firstChar == '.' && length == 1) {
        token = Token(source, Token::Category::KEYWORD, Token::Type::FINAL);
    } else if (firstChar == ':' &&
               (length == 1 || (length == 2 && code.getCharacter(currentLine, start + length - 1) == '='))) {
        token = Token(source, Token::Category::OPERATOR, Token::Type::ASSIGNMENT);
    } else if (firstChar == ',' && length == 1) {
        token = Token(source, Token::Category::SEPARATOR, Token::Type::COLON);
    } else if (firstChar == ';' && length == 1) {
        token = Token(source, Token::Category::SEPARATOR, Token::Type::SEMICOLON);
    } else if (firstChar == '(' && length == 1) {
        token = Token(source, Token::Category::SEPARATOR, Token::Type::LEFT_BRACKET);
    } else if (firstChar == ')' && length == 1) {
        token = Token(source, Token::Category::SEPARATOR, Token::Type::RIGHT_BRACKET);
    } else if (isdigit(firstChar)) {
        for (unsigned i = 0; i < length; i++) {
            if (!isdigit(code.getCharacter(currentLine, start + i))) {
                token = Token(source, Token::Category::INVALID, Token::Type::NOT_USED);
                return -1;
            }
        }
        token = Token(source, Token::Category::LITERAL, Token::Type::NOT_USED);
    } else {
        for (unsigned i = 0; i < length; i++) {
            if (!isalpha(code.getCharacter(currentLine, start + i))) {
                token = Token(source, Token::Category::INVALID, Token::Type::NOT_USED);
                return -1;
            }
        }

        std::string tokenAsString = std::string(code.getLine(currentLine)).substr(start,
                                                                                  length); // TODO maybe use string_view instead

        if (tokenAsString == "PARAM") {
            token = Token(source, Token::Category::KEYWORD, Token::Type::PARAM);
        } else if (tokenAsString == "VAR") {
            token = Token(source, Token::Category::KEYWORD, Token::Type::VAR);
        } else if (tokenAsString == "CONST") {
            token = Token(source, Token::Category::KEYWORD, Token::Type::CONST);
        } else if (tokenAsString == "BEGIN") {
            token = Token(source, Token::Category::KEYWORD, Token::Type::BEGIN);
        } else if (tokenAsString == "END") {
            token = Token(source, Token::Category::KEYWORD, Token::Type::END);
        } else if (tokenAsString == "RETURN") {
            token = Token(source, Token::Category::KEYWORD, Token::Type::RETURN);
        } else {
            token = Token(source, Token::Category::IDENTIFIER, Token::Type::NOT_USED);
        }
    }
    return 1;
}
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------