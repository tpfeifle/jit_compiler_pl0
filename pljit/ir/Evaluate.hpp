#pragma once

#include <unordered_map>
#include <pljit/ast/SymbolTable.hpp>
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
class Evaluate {
public:
    explicit Evaluate(std::unordered_map<std::string, pljit::ast::Symbol> &symbolTable){
        for(const auto &variable: symbolTable) {
            variables.emplace(std::pair<std::string, int>(variable.first, 0)); //TODO check if all should be initialized with zero (also vars?
        }
    }
    std::unordered_map<std::string, int> variables {};
    unsigned errorCode = 0;
    // TODO some state whether or not there was an error should be here (requirements)
};
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------