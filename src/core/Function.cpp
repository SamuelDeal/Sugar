#include "Function.h"
#include "../utils/Getter.tpl.h"

namespace sugar {
namespace core {


Function::Function(const std::string &name, Type* returnType, const std::list<const Type *> &argTypes,
                   std::function<llvm::Function * ()> fnDeclGenerator,
                   std::function<bool (llvm::Function*)> fnImplGenerator):
    AbstractFunction(fnDeclGenerator, fnImplGenerator, returnType, argTypes)
{
    _name = name;
}

Function::Function(const std::string &name, Type* returnType, const std::list<const Type *> &argTypes, ast::Node *fnDeclNode,
                   std::function<llvm::Function * ()> fnDeclGenerator):
    AbstractFunction(fnDeclGenerator, fnDeclNode, returnType, argTypes)
{
    _name = name;
}

Function::Function(const std::string &name, NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes):
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
