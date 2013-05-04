#include "Operator.h"

namespace sugar {
namespace core {

Operator::Operator(int operatorId, llvm::Function *fn, Type *returnType, const std::list<const Type *> &argTypes):
    AbstractFunction(fn, returnType, argTypes)
{
    _operatorId = operatorId;
}

Operator::Operator(int operatorId, gen::NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes):
    AbstractFunction(fn, returnType, argTypes)
{
    _operatorId = operatorId;
}

Operator::~Operator(){
}

int Operator::getOperatorId() const{
    return _operatorId;
}


} // namespace core
} // namespace sugar
