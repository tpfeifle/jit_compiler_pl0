#pragma once

#include <string>
#include <pljit/source/SourceCode.hpp>
#include <pljit/ast/ASTNode.hpp>
#include <pljit/ast/SemanticAnalyzer.hpp>
#include <pljit/ir/OptimizeConstPropagation.hpp>
#include <pljit/ir/OptimizeDeadCode.hpp>
#include <pljit/lexer/Lexer.hpp>
#include <pljit/parser/Parser.hpp>
#include <mutex>
#include "PljitFunction.hpp"
//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
class FunctionHandle {
    PljitFunction* function;
public:
    template<typename... Sizes>
    int operator()(Sizes... sizes) {
        return function->execute(sizes...);
    }
    FunctionHandle(PljitFunction* function): function(function) {};
};

class Pljit {
public:
    FunctionHandle registerFunction(std::vector<std::string> codeText);
    std::vector<std::unique_ptr<PljitFunction>> functions;
};
//---------------------------------------------------------------------------
} // namespace pljit_function
//---------------------------------------------------------------------------