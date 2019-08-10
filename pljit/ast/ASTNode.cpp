#include "ASTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit {
//---------------------------------------------------------------------------
namespace ast {
//---------------------------------------------------------------------------
ASTNode::Type ASTNode::getType() const {
    return type;
}
//---------------------------------------------------------------------------
int FunctionAST::execute(pljit::Evaluate& evaluate) {
    for (auto&& child: children) {
        child->execute(evaluate);
    }
    return evaluate.variables.at("Return");
}
//---------------------------------------------------------------------------
int LiteralAST::execute(pljit::Evaluate& evaluate) {
    return value;
}
//---------------------------------------------------------------------------
int UnaryAST::execute(pljit::Evaluate& evaluate) {
    if (sign == SignType::Plus) {
        return child->execute(evaluate);
    } else {
        return -1 * child->execute(evaluate);
    }
}
//---------------------------------------------------------------------------
int IdentifierAST::execute(pljit::Evaluate& evaluate) {
    int temp = evaluate.variables.at(identifier); // TODO
    return temp;
}
//---------------------------------------------------------------------------

int AssignmentAST::execute(pljit::Evaluate& evaluate) {
    evaluate.variables[identifier->identifier] = expression->execute(evaluate);
    return 0; // TODO not needed
}
//---------------------------------------------------------------------------
int ReturnStatementAST::execute(pljit::Evaluate& evaluate) {
    evaluate.variables.insert({"Return", expression->execute(evaluate)});
    return 0; // TODO: actually not really needed
}
//---------------------------------------------------------------------------
int BinaryOperationAST::execute(pljit::Evaluate& evaluate) {
    switch (type) {
        case OperationType::Plus:
            return left->execute(evaluate) + right->execute(evaluate);
        case OperationType::Minus:
            return left->execute(evaluate) - right->execute(evaluate);
        case OperationType::Multiply:
            return left->execute(evaluate) * right->execute(evaluate);
        case OperationType::Divide:
            int temp = right->execute(evaluate);
            if (temp == 0) {
                std::cout << "Division by zero error" << std::endl;
                evaluate.errorCode = 1;
            }
            return left->execute(evaluate) / temp;
    }
}
} //namespace ast
//---------------------------------------------------------------------------
} // namespace pljit
//---------------------------------------------------------------------------
