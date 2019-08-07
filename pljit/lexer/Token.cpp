#include "../source/SourceReference.hpp"
#include "../lexer/Token.hpp"
#include <optional>

namespace pljit {
Token::Type Token::getType() const {
    return type;
}
}