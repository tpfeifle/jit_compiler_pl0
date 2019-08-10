#pragma once

#include <map>
#include <pljit/source/SourceReference.hpp>

namespace pljit {
//---------------------------------------------------------------------------
namespace ast {
//---------------------------------------------------------------------------
class Symbol {
public:
    enum Type {
        Const,
        Var,
        Param
    };
    Type type;
    SourceReference reference;
    bool initialized;
    int value;
    // TODO: store the value here as well?

    Symbol(Type type, SourceReference reference, bool initialized, int value) : type(type), reference(std::move(reference)),
                                                                     initialized(initialized), value(value) {}
};

//---------------------------------------------------------------------------
/*class SymbolTable {
public:
    std::map<std::string, Symbol> declarations;
};*/
//---------------------------------------------------------------------------
} //namespace ast
//---------------------------------------------------------------------------
} //namespace pljit
//---------------------------------------------------------------------------