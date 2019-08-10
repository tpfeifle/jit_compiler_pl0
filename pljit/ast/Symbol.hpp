#pragma once

#include <map>
#include <pljit/source/SourceReference.hpp>

namespace pljit_ast {
//---------------------------------------------------------------------------
class Symbol {
public:
    enum Type {
        Const,
        Var,
        Param
    };
    Type type;
    pljit_source::SourceReference reference;
    bool initialized;
    int64_t value;

    Symbol(Type type, pljit_source::SourceReference reference, bool initialized, int64_t value) : type(type), reference(std::move(reference)),
                                                                     initialized(initialized), value(value) {}
};
//---------------------------------------------------------------------------
} //namespace pljit_ast
//---------------------------------------------------------------------------