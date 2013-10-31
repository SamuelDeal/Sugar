#include "Operator.h"

namespace sugar {
namespace core {

Operator::Operator(int operatorId, llvm::Function *fn, Type *returnType, const std::list<const Type *> &argTypes):
    AbstractFunction(fn, returnType, argTypes)
{
    _operatorId = operatorId;
    _before = false;
}

Operator::Operator(int operatorId, NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes):
    AbstractFunction(fn, returnType, argTypes)
{
    _operatorId = operatorId;
    _before = false;
}

Operator::Operator(int operatorId, llvm::Function *fn, Type* returnType, const Type * type, bool before) :
    AbstractFunction(fn, returnType, std::list<const Type*>(1, type))
{
    _operatorId = operatorId;
    _before = before;
}

Operator::Operator(int operatorId, NativeFunction fn, Type* returnType, const Type * type, bool before) :
    AbstractFunction(fn, returnType, std::list<const Type*>(1, type))
{
    _operatorId = operatorId;
    _before = before;
}

Operator::~Operator(){
}

int Operator::getOperatorId() const{
    return _operatorId;
}

bool Operator::getBefore() const{
    return _before;
}


} // namespace core
} // namespace sugar
