#ifndef SUGAR_AST_COMPARISON_H
#define SUGAR_AST_COMPARISON_H

#include <list>

#include "Expression.h"

namespace sugar {
namespace ast {

class Comparison : public Expression
{
public:
    enum Kind {
        eEquality
    };

    Comparison(Kind kind, std::list<Expression *> *expressions);
    virtual ~Comparison();

    Kind kind;
    std::list<Expression *> *expressions;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_COMPARISON_H
