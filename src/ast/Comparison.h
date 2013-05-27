#ifndef SUGAR_AST_COMPARISON_H
#define SUGAR_AST_COMPARISON_H

#include <list>

#include "Expression.h"

namespace sugar {
namespace ast {

class Comparison : public Expression
{
public:
    Comparison(int operatorId, std::list<Expression *> *expressions);
    virtual ~Comparison();

    int operatorId;
    std::list<Expression *> *expressions;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_COMPARISON_H
