#include "PTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit::parser {
//---------------------------------------------------------------------------
PTNode::Type PTNode::getType() const {
    return type;
};
//---------------------------------------------------------------------------
OperatorAlternationPTNode::OperatorType OperatorAlternationPTNode::getOperatorType() const {
    return operatorType;
}
//---------------------------------------------------------------------------
int LiteralPTNode::getValue() const {
    return value;
}
//---------------------------------------------------------------------------
/*std::vector<std::unique_ptr<PTNode>> NonTerminalPTNode::getChildren() const {
    return children;
}*/
//---------------------------------------------------------------------------
} // namespace pljit::parser
//---------------------------------------------------------------------------