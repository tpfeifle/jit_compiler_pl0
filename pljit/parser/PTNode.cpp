#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit::parser {
//---------------------------------------------------------------------------
PTNode::Type PTNode::getType() const {
    return type;
}
//---------------------------------------------------------------------------
OperatorAlternationPTNode::OperatorType OperatorAlternationPTNode::getOperatorType() const {
    return operatorType;
}
//---------------------------------------------------------------------------
int64_t LiteralPTNode::getValue() const {
    return value;
}
//---------------------------------------------------------------------------
void LiteralPTNode::accept(PTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void IdentifierPTNode::accept(PTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void GenericTokenPTNode::accept(PTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void NonTerminalPTNode::accept(PTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
/*std::vector<std::unique_ptr<PTNode>> NonTerminalPTNode::getChildren() const {
    return children;
}*/
//---------------------------------------------------------------------------
} // namespace pljit::parser
//---------------------------------------------------------------------------