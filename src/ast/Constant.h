#ifndef SUGAR_AST_CONSTANT_H
#define SUGAR_AST_CONSTANT_H

#include "Expression.h"

namespace sugar {
namespace ast {

class Constant : public Expression
{
public:
    enum ConstantType {
        eBoolean,
        eInteger,
        eFloat
    };

    Constant(long long integerVal);
    Constant(double floatVal);
    Constant(bool boolVal);

    union {
        long long integerValue;
        double floatValue;
        bool boolValue;
    } value;

    ConstantType getConstantType() const;

protected:
    ConstantType _constantType;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_CONSTANT_H
