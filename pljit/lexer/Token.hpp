#pragma once
//---------------------------------------------------------------------------
#include <pljit/source/SourceReference.hpp>
//---------------------------------------------------------------------------
namespace pljit_lexer {
//---------------------------------------------------------------------------
/// Represents a token produced by the lexer from the source code
class Token {
public:
    /// The types of nodes in the token-stream from the lexer
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
        Invalid
    };

    /// Constructor
    Token(pljit_source::SourceReference source, const Type type) : source(source), type(type) {}
    /// Get the type of the Token
    [[nodiscard]] Token::Type getType() const;

    /// A reference to the represented source code
    pljit_source::SourceReference source;
private:
    Type type;
};
//---------------------------------------------------------------------------
} // namespace pljit_lexer
//---------------------------------------------------------------------------