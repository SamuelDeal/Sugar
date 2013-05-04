#include "Function.h"

namespace sugar {
namespace core {

Function::Function(const std::string &name, llvm::Function *fn, Type *returnType, const std::list<const Type *> &argTypes):
    AbstractFunction(fn, returnType, argTypes)
{
    _name = name;
}

Function::Function(const std::string &name, gen::NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes):
    AbstractFunction(fn, returnType, argTypes)
{
    _name = name;
}

Function::~Function(){
}

const std::string& Function::getName() const{
    return _name;
}

} // namespace core
} // namespace sugar
