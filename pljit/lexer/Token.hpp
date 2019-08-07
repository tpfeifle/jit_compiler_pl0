#pragma once

#include "../source/SourceReference.hpp"
#include <optional>
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
class Token {
public:
    enum Category {
        IDENTIFIER,
        LITERAL,
        OPERATOR,
        SEPARATOR,
        KEYWORD,
        INVALID
    };

    enum Type {
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        ASSIGNMENT,
        EQUAL,
        COLON,
        SEMICOLON,
        PARAM,
        RETURN,
        BEGIN,
        END,
        CONST,
        VAR,
        FINAL,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        NOT_USED
    };

    Token(SourceReference source, const Category tokenCategory, const Type type) : category(tokenCategory), type(type),
                                                                                   source(source) {}

    Token(Token& token) = default;
    Token(Token&& token) = default;
    Token& operator=(Token&& other) = default;
    Token::Type getType() const;
    // Token::Type getCategory() const; TODO check if category really needed

    // virtual ~Token() = default; // public virtual destructor
    Category category;
    SourceReference source;
private:
    Type type;
};
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------

/*

struct OperatorToken : Token {
    enum Type {
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE
    };
    OperatorToken(SourceReference source, Type type) : Token(source, Token::Category::OPERATOR), type(type) {};
    Type type;
};

struct SeparatorToken : Token {
    enum Type {
        Colon,
        Semicolon,
    };
    SeparatorToken(SourceReference source, Type type) : Token(source, Token::Category::SEPARATOR), type(type) {};
    Type type;
};

struct KeywordToken : Token {
    enum Type {
        PARAM,
        RETURN,
        BEGIN,
        END,
        CONST,
        VAR,
        FINAL
    };
    KeywordToken(SourceReference source, Type type) : Token(source, Token::Category::KEYWORD), type(type) {};
    Type type;
};*/