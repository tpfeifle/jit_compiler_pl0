#pragma once

#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit {
class DotPTVisitor : public Visitor {
    std::string nodeTypeToString(PTNode::Type type);

public:
    void visitPTNode(PTNode* node);
};
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------