#include "Operator.h"

namespace sugar {
namespace core {



Operator::Operator(int operatorId, Type* returnType, const std::list<const Type *> &argTypes,
                   std::function<llvm::Function * ()> fnDeclGenerator,
                   std::function<bool (llvm::Function*)> fnImplGenerator):
    AbstractFunction(fnDeclGenerator, fnImplGenerator, returnType, argTypes)
{
    _operatorId = operatorId;
    _before = false;
}

Operator::Operator(int operatorId, Type* returnType, const std::list<const Type *> &argTypes, ast::Node *fnDeclNode,
                   std::function<llvm::Function * ()> fnDeclGenerator):
    AbstractFunction(fnDeclGenerator, fnDeclNode, returnType, argTypes)
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

Operator::Operator(int operatorId, Type* returnType, const Type * type, bool before,
                   std::function<llvm::Function * ()> fnDeclGenerator,
                   std::function<bool (llvm::Function*)> fnImplGenerator):
    AbstractFunction(fnDeclGenerator, fnImplGenerator, returnType, std::list<const Type*>(1, type))
{
    _operatorId = operatorId;
    _before = before;
}

Operator::Operator(int operatorId, Type* returnType, const Type * type, bool before, ast::Node *fnDeclNode,
                   std::function<llvm::Function * ()> fnDeclGenerator):
    AbstractFunction(fnDeclGenerator, fnDeclNode, returnType, std::list<const Type*>(1, type))
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
