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
    // TODO: store the value here as well?

    Symbol(Type type, SourceReference reference, bool initialized) : type(type), reference(std::move(reference)),
                                                                     initialized(initialized) {}
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