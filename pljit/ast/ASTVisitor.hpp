#pragma once
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <pljit/ir/EvalContext.hpp>
#include "ASTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
class ASTVisitor {
public:
    virtual void visit(FunctionAST& node) = 0;
    virtual void visit(AssignmentAST& node) = 0;
    virtual void visit(ReturnStatementAST& node) = 0;
    virtual void visit(LiteralAST& node) = 0;
    virtual void visit(IdentifierAST& node) = 0;
    virtual void visit(UnaryAST& node) = 0;
    virtual void visit(BinaryOperationAST& node) = 0;
};
//---------------------------------------------------------------------------
}  // namespace pljit_ast
//---------------------------------------------------------------------------
