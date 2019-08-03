#include <map>
#include <pljit/source/SourceReference.hpp>

#pragma once


class Symbol {
public:
    enum Type {
        Const,
        Var,
        Param
    };
    Type type;
    SourceReference reference;
    // TODO: store the value here as well?

    Symbol(Type type, SourceReference reference) : type(type), reference(std::move(reference)) {}
};

class SymbolTable {
public:
    std::map<std::string, Symbol> declarations;
};