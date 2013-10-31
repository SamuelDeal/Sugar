#include "AbstractFunction.h"

//FIXME: remove this
//#include <iostream>
#include "Type.h"

namespace sugar {
namespace core {

AbstractFunction::AbstractFunction(llvm::Function *fn, Type* returnType, const std::list<const Type *> &argTypes):
    _argsTypes(argTypes), _returnType(returnType){
    _native = false;
    _impl.llvmFunction = fn;
}

AbstractFunction::AbstractFunction(NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes):
    _argsTypes(argTypes), _returnType(returnType){
    _native = true;
    _impl.nativeFunction = fn;
}

AbstractFunction::~AbstractFunction(){
}

bool AbstractFunction::isNative() const{
    return _native;
}

bool AbstractFunction::match(const std::list<const Type *> &argsTypes, const CastGraph &castGraph) const {
    std::list<const Type *>::const_iterator mine = _argsTypes.begin();
    std::list<const Type *>::const_iterator asked = argsTypes.begin();

    while(asked != argsTypes.end()){
        if(mine == _argsTypes.end()){
            return false;
        }
        if(*mine != *asked){
            return false;
        }
        ++asked;
        ++mine;
    }
    return mine == _argsTypes.end();
}

AbstractFunction::operator llvm::Function*() const{
    if(_native){
        return NULL;
    }
    return _impl.llvmFunction;
}

NativeFunction AbstractFunction::getNative() const {
    return _impl.nativeFunction;
}

const Type* AbstractFunction::getReturnType() const {
    return _returnType;
}

} // namespace core
} // namespace sugar
