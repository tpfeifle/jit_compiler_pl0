#pragma once

#include <map>
#include <pljit/source/SourceReference.hpp>

namespace pljit::ast {
//---------------------------------------------------------------------------
class Symbol {
public:
    enum Type {
        Const,
        Var,
        Param
    };
    Type type;
    source::SourceReference reference;
    bool initialized;
    int64_t value;
    // TODO: store the value here as well?

    Symbol(Type type, source::SourceReference reference, bool initialized, int64_t value) : type(type), reference(std::move(reference)),
                                                                     initialized(initialized), value(value) {}
};
//---------------------------------------------------------------------------
} //namespace pljit::ast
//---------------------------------------------------------------------------