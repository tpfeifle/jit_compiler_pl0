#pragma once

#include <optional>
#include <pljit/source/SourceReference.hpp>
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

    Token(source::SourceReference source, const Type type) : source(std::move(source)), type(type) {}
    Token(Token& token) = default;
    Token(Token&& token) = default;

    Token& operator=(Token&& other) = default;
    [[nodiscard]] Token::Type getType() const;

    // virtual ~Token() = default; // public virtual destructor
    source::SourceReference source;
private:
    Type type;
};
//---------------------------------------------------------------------------
} // namespace pljit::lexer
//---------------------------------------------------------------------------