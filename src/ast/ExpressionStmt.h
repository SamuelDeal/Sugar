#ifndef SUGAR_AST_EXPRESSIONSTMT_H
#define SUGAR_AST_EXPRESSIONSTMT_H

#include "Statement.h"

namespace sugar {
namespace ast {

class Expression;

class ExpressionStmt : public Statement
{
public:
    ExpressionStmt(Expression *expression);
    virtual ~ExpressionStmt();
    Expression* expression;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_EXPRESSIONSTMT_H
