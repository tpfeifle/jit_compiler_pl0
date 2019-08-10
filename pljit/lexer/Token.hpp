#pragma once

#include "../source/SourceReference.hpp"
#include <optional>
//---------------------------------------------------------------------------
namespace pljit::lexer {
//---------------------------------------------------------------------------
class Token {
public:
    enum Type {
        Plus,
        Minus,
        Multiply,
        Divide,
        Assignment,
        Equal,
        Colon,
        Semicolon,
        Param,
        Return,
        Begin,
        End,
        Const,
        Var,
        Final,
        Left_Bracket,
        Right_Bracket,
        Identifier,
        Literal,
        NOT_USED //TODO
    };

    Token(SourceReference source, const Type type) : source(source), type(type) {}
    Token(Token& token) = default;
    Token(Token&& token) = default;

    Token& operator=(Token&& other) = default;
    [[nodiscard]] Token::Type getType() const;

    // virtual ~Token() = default; // public virtual destructor
    SourceReference source;
private:
    Type type;
};
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------