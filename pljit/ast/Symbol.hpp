#pragma once
//---------------------------------------------------------------------------
#include <map>
#include <pljit/source/SourceReference.hpp>
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
/// Represents a symbol in the symbol table
class Symbol {
public:
    /// The different types of symbols that exist
    enum Type {
        Const,
        Var,
        Param
    };
    Type type;

    /// A reference to the source-code
    pljit_source::SourceReference reference;

    /// Flag whether the symbol is already initialized
    bool initialized;

    /// Literal value of the symbol
    int64_t value;

    /// Constructor
    Symbol(Type type, pljit_source::SourceReference reference, bool initialized, int64_t value) :
            type(type), reference(reference), initialized(initialized), value(value) {}
};
//---------------------------------------------------------------------------
} //namespace pljit_ast
//---------------------------------------------------------------------------