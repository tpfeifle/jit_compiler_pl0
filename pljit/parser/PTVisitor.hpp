#pragma once
//---------------------------------------------------------------------------
#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
/// Visitor for the parse-tree, used to output e.g. the tree in the dot-format
class PTVisitor {
public:
    /// Visitor methods for the visitor pattern
    virtual void visit(LiteralPTNode& node) = 0;
    virtual void visit(IdentifierPTNode& node) = 0;
    virtual void visit(GenericTokenPTNode& node) = 0;
    virtual void visit(NonTerminalPTNode& node) = 0;
};
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------