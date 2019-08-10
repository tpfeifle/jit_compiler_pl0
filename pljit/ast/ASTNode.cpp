#include "ASTNode.hpp"
//---------------------------------------------------------------------------
namespace pljit::ast {
//---------------------------------------------------------------------------
ASTNode::Type ASTNode::getType() const {
    return type;
}
//---------------------------------------------------------------------------
int64_t FunctionAST::execute(ir::Evaluate& evaluate) {
    for (auto&& child: children) {
        child->execute(evaluate);
    }
    return evaluate.variables.at("Return");
}
//---------------------------------------------------------------------------
int64_t LiteralAST::execute(ir::Evaluate&) {
    return value;
}
//---------------------------------------------------------------------------
int64_t UnaryAST::execute(ir::Evaluate& evaluate) {
    if (sign == SignType::Plus) {
        return child->execute(evaluate);
    } else {
        return -1 * child->execute(evaluate);
    }
}
//---------------------------------------------------------------------------
int64_t IdentifierAST::execute(ir::Evaluate& evaluate) {
    return evaluate.variables.at(identifier); // TODO
}
//---------------------------------------------------------------------------

int64_t AssignmentAST::execute(ir::Evaluate& evaluate) {
    evaluate.variables[identifier->identifier] = expression->execute(evaluate);
    return 0; // TODO not needed
}
//---------------------------------------------------------------------------
int64_t ReturnStatementAST::execute(ir::Evaluate& evaluate) {
    evaluate.variables.insert({"Return", expression->execute(evaluate)});
    return 0; // TODO: actually not really needed
}
//---------------------------------------------------------------------------
int64_t BinaryOperationAST::execute(ir::Evaluate& evaluate) {
    switch (type) {
        case OperationType::Plus:
            return left->execute(evaluate) + right->execute(evaluate);
        case OperationType::Minus:
            return left->execute(evaluate) - right->execute(evaluate);
        case OperationType::Multiply:
            return left->execute(evaluate) * right->execute(evaluate);
        case OperationType::Divide:
            int64_t temp = right->execute(evaluate);
            if (temp == 0) {
                std::cout << "Division by zero error" << std::endl;
                evaluate.errorCode = 1;
            }
            return left->execute(evaluate) / temp;
    }
}
//---------------------------------------------------------------------------
void FunctionAST::accept(ASTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void LiteralAST::accept(ASTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void UnaryAST::accept(ASTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void IdentifierAST::accept(ASTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void AssignmentAST::accept(ASTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void ReturnStatementAST::accept(ASTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
void BinaryOperationAST::accept(ASTVisitor& v) {
    v.visit(*this);
}
//---------------------------------------------------------------------------
} // namespace pljit::ast
//---------------------------------------------------------------------------
