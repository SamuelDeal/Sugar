#include "ExpressionStmt.h"

#include "Expression.h"

namespace sugar {
namespace ast {

ExpressionStmt::ExpressionStmt(Expression* expression) : Statement(Node::eExpressionStmt), expression(expression)
{
}

ExpressionStmt::~ExpressionStmt(){
    delete expression;
}

} // namespace ast
} // namespace sugar
