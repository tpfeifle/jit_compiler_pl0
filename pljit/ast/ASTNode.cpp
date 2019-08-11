#include "ASTNode.hpp"
#include "ASTVisitor.hpp"
//---------------------------------------------------------------------------
namespace pljit_ast {
//---------------------------------------------------------------------------
ASTNode::Type ASTNode::getType() const {
    return type;
}
//---------------------------------------------------------------------------
int64_t LiteralAST::getValue() const {
    return value;
}
//---------------------------------------------------------------------------
int64_t FunctionAST::execute(pljit_ir::EvalContext& evaluate)
// Get the return value of the function in the provided context
{
    for (auto&& child: children) {
        child->execute(evaluate);
    }
    return evaluate.parameters.at("Return");
}
//---------------------------------------------------------------------------
int64_t LiteralAST::execute(pljit_ir::EvalContext&)
// Get the value of the literal
{
    return value;
}
//---------------------------------------------------------------------------
int64_t UnaryAST::execute(pljit_ir::EvalContext& evaluate)
// Get the return value of the unary node in the provided context
{
    if (sign == SignType::Plus) {
        return child->execute(evaluate);
    } else {
        return -1 * child->execute(evaluate);
    }
}
//---------------------------------------------------------------------------
int64_t IdentifierAST::execute(pljit_ir::EvalContext& evaluate)
// Get the value of the identifier in the provided context
{
    return evaluate.parameters.at(identifier);
}
//---------------------------------------------------------------------------

int64_t AssignmentAST::execute(pljit_ir::EvalContext& evaluate)
// Update the evaluation context with the assignment
{
    evaluate.parameters[identifier->identifier] = expression->execute(evaluate);
    return 0;
}
//---------------------------------------------------------------------------
int64_t ReturnStatementAST::execute(pljit_ir::EvalContext& evaluate)
// Update the evaluation context with the return value
{
    evaluate.parameters.insert({"Return", expression->execute(evaluate)});
    return 0;
}
//---------------------------------------------------------------------------
int64_t BinaryOperationAST::execute(pljit_ir::EvalContext& evaluate)
// Return the result of the binary operation in the provided context
{
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
                std::cout << "Division by zero error" << std::endl; // TODO
                evaluate.errorCode = 1;
                return 0;
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
} // namespace pljit_ast
//---------------------------------------------------------------------------
