#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
PTNode::Type PTNode::getType() const {
// Get the specific parse-tree node type
    return type;
}
//---------------------------------------------------------------------------
OperatorAlternationPTNode::OperatorType OperatorAlternationPTNode::getOperatorType() const {
// Get the specific operator type of the operator node
    return operatorType;
}
//---------------------------------------------------------------------------
int64_t LiteralPTNode::getValue() const {
// Get the specific value of the literal node
    return value;
}
//---------------------------------------------------------------------------
std::string IdentifierPTNode::getName() const {
// Get the specific name of the identifier node
    return std::string(source.getText());
}
//---------------------------------------------------------------------------
void LiteralPTNode::accept(PTVisitor& v) {
// Accept method for the visitor pattern
    v.visit(*this);
}
//---------------------------------------------------------------------------
void IdentifierPTNode::accept(PTVisitor& v) {
// Accept method for the visitor pattern
    v.visit(*this);
}
//---------------------------------------------------------------------------
void GenericTokenPTNode::accept(PTVisitor& v) {
// Accept method for the visitor pattern
    v.visit(*this);
}
//---------------------------------------------------------------------------
void NonTerminalPTNode::accept(PTVisitor& v) {
// Accept method for the visitor pattern
    v.visit(*this);
}
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------