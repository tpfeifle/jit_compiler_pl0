#pragma once
//---------------------------------------------------------------------------
#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit_parser {
/// Implements Visitor to output the Parse-Tree in the dot-syntax
class DotPTVisitor : public PTVisitor {
    /// Returns the parse-tree node-types as a string
    static std::string nodeTypeToString(PTNode::Type type);
    /// Identifier representing the currently visited node
    unsigned node_count = 0;

public:
    /// Visitor methods for the different parse-tree nodes
    void visit(IdentifierPTNode& node) override;
    void visit(LiteralPTNode& node) override;
    void visit(GenericTokenPTNode& node) override;
    void visit(NonTerminalPTNode& node) override;
};
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------