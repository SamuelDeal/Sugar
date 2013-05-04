#include "Cast.h"

namespace sugar {
namespace core {

std::list<const Type *> createList(Type* fromType) {
    std::list<const Type *> result;
    result.push_back(fromType);
    return result;
}

Cast::Cast(llvm::Function *fn, Type* fromType, Type* toType):
    AbstractFunction(fn, toType, createList(fromType))
{}

Cast::Cast(gen::NativeFunction fn, Type* fromType, Type* toType):
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
