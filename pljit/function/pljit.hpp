#pragma once
//---------------------------------------------------------------------------
#include <string>
#include "PljitFunction.hpp"
//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
/// A lightweight handle for the functions stored in Pljit
class FunctionHandle {
    /// Pointer to the function
    PljitFunction* function;
public:

    /// Call operator of the handle
    template<typename... Params>
    int operator()(Params... params) {
        return function->execute(params...);
    }

    /// Constructor
    FunctionHandle(PljitFunction* function) : function(function) {};
};
//---------------------------------------------------------------------------
/// JIT-class: handles registering functions and their source code
class Pljit {
public:
    /// Interface to register new functions with their source code
    FunctionHandle registerFunction(const std::string& codeText);

    /// Store for the functions
    std::vector<std::unique_ptr<PljitFunction>> functions;
};
//---------------------------------------------------------------------------
} // namespace pljit_function
//---------------------------------------------------------------------------