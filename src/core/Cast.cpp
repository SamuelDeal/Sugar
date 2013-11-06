#include "Cast.h"

namespace sugar {
namespace core {

std::list<const Type *> createList(Type* fromType) {
    std::list<const Type *> result;
    result.push_back(fromType);
    return result;
}

Cast::Cast(Type* fromType, Type* toType,
                   std::function<llvm::Function * ()> fnDeclGenerator,
                   std::function<void (llvm::Function*)> fnImplGenerator):
    AbstractFunction(fnDeclGenerator, fnImplGenerator, toType, createList(fromType))
{}

Cast::Cast(Type* fromType, Type* toType, ast::Node *fnDeclNode,
                   std::function<llvm::Function * ()> fnDeclGenerator):
    AbstractFunction(fnDeclGenerator, fnDeclNode, toType, createList(fromType))
{}

Cast::Cast(NativeFunction fn, Type* fromType, Type* toType):
    AbstractFunction(fn, toType, createList(fromType))
{}

Cast::~Cast(){
}

const Type* Cast::getFrom() const{
    return *_argsTypes.begin();
}

const Type* Cast::getTo() const{
    return _returnType;
}

} // namespace core
} // namespace sugar
