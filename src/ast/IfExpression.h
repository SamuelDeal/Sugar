#ifndef SUGAR_AST_IFEXPRESSION_H
#define SUGAR_AST_IFEXPRESSION_H

#include "Expression.h"

#include "../utils.h"

namespace sugar {
namespace ast {

class Block;

class IfExpression : public Expression
{
public:
    IfExpression(Expression *cond, Block *thenBlk, Block *elseBlk = NULL);
    virtual ~IfExpression();

    Expression *cond;
    Block *thenBlk;
    Block *elseBlk;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_IFEXPRESSION_H
