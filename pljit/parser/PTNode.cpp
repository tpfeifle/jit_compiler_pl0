#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit_parser {
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
std::string IdentifierPTNode::getName() const {
    return source.getText();
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
} // namespace pljit_parser
//---------------------------------------------------------------------------