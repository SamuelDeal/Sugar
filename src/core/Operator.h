#ifndef SUGAR_CORE_OPERATOR_H
#define SUGAR_CORE_OPERATOR_H

#include "AbstractFunction.h"

namespace sugar {
namespace core {

class Operator : public AbstractFunction
{
public:
    Operator(int operatorId, llvm::Function *fn, Type* returnType, const std::list<const Type *> &argTypes);
    Operator(int operatorId, gen::NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes);

    virtual ~Operator();
    int getOperatorId() const;

protected:
    int _operatorId;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_OPERATOR_H
