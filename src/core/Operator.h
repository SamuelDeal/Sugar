#ifndef SUGAR_CORE_OPERATOR_H
#define SUGAR_CORE_OPERATOR_H

#include "AbstractFunction.h"

namespace sugar {
namespace core {

class Operator : public AbstractFunction
{
public:
    Operator(int operatorId, llvm::Function *fn, Type* returnType, const std::list<const Type *> &argTypes);
    Operator(int operatorId, NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes);

    Operator(int operatorId, llvm::Function *fn, Type* returnType, const Type * type, bool before);
    Operator(int operatorId, NativeFunction fn, Type* returnType, const Type * type, bool before);

    virtual ~Operator();
    int getOperatorId() const;
    bool getBefore() const;

protected:
    int _operatorId;
    bool _before;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_OPERATOR_H
