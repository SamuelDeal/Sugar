#include "ReturnStmt.h"

namespace sugar {
namespace ast {

ReturnStmt::ReturnStmt(Expression *expression) : Statement(Node::eReturnStmt), expression(expression)
{
}

ReturnStmt::~ReturnStmt() {
    delete expression;
}

} // namespace ast
} // namespace sugar
