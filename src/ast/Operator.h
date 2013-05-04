#ifndef SUGAR_AST_OPERATOR_H
#define SUGAR_AST_OPERATOR_H

#include "Expression.h"

namespace sugar {
namespace ast {

class Operator : public Expression
{
public:
    Operator(int operatorId, std::list<Expression*> *expressions);
    virtual ~Operator();

    std::list<Expression*> *args;
    int operatorId;

    virtual bool isImplicitFunctionCall() const;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_OPERATOR_H
