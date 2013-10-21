#ifndef SUGAR_AST_COMPARISON_H
#define SUGAR_AST_COMPARISON_H

#include <list>

#include "Expression.h"

namespace sugar {
namespace ast {

class Comparison : public Expression
{
public:
    Comparison(Expression *firstExpr);
    virtual ~Comparison();

    void add(int operatorId, Expression *expression);
    std::list<Expression *> expressions;
    std::list<int> operators;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_COMPARISON_H
