#pragma once

#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit_parser {
class DotPTVisitor : public PTVisitor {
    std::string nodeTypeToString(PTNode::Type type);
    unsigned node_count = 0;

public:
    void visit(IdentifierPTNode& node) override;
    void visit(LiteralPTNode& node) override;
    void visit(GenericTokenPTNode& node) override;
    void visit(NonTerminalPTNode& node) override;
};
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------