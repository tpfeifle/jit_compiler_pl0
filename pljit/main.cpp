#include <iostream>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ir/OptimizeDeadCode.hpp>
#include <pljit/ir/OptimizeConstPropagation.hpp>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "parser/PTNode.hpp"
#include "ast/AST.hpp"
#include "pljit.hpp"
#include "ast/DotASTVisitor.hpp"
#include "ir/Evaluate.hpp"

//---------------------------------------------------------------------------
using namespace std;
using namespace pljit;
//---------------------------------------------------------------------------
int main() {
    Pljit jit; // Create an object that manages just-in-time
    // compilation (jit) of functions
    // Register a function with its source with the jit object
    auto func = jit.registerFunction({"PARAM hm, loft;\n",
                                     "BEGIN\n",
                                     "    hm := 12 + hm;\n",
                                     "    RETURN loft + (1 + 2)\n",
                                     "END.\n"});
    // Call the PL/0 function with the arguments 123 for a and 456 for b
    auto result = func(123, 456);
    std::cout << "Result:" << result << std::endl;
}
//---------------------------------------------------------------------------
