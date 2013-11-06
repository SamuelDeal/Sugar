#include "AbstractFunction.h"

#include "Type.h"

namespace sugar {
namespace core {

AbstractFunction::AbstractFunction(std::function<llvm::Function * ()> fnDeclGenerator,
                                   std::function<void (llvm::Function*)> fnImplGenerator,
                                   Type* returnType, const std::list<const Type *> &argTypes):
    _argsTypes(argTypes), _returnType(returnType){
    _native = false;
    _impl.irFunction.llvmFunction = NULL;
    _impl.irFunction.fnDeclGenerator = fnDeclGenerator;
    _impl.irFunction.fnImplGenerator = fnImplGenerator;
    _impl.irFunction.funcDeclNode = NULL;

}

AbstractFunction::AbstractFunction(std::function<llvm::Function * ()> fnDeclGenerator, ast::Node *funcDecl,
                                   Type* returnType, const std::list<const Type *> &argTypes):
    _argsTypes(argTypes), _returnType(returnType){
    _native = false;
    _impl.irFunction.llvmFunction = NULL;
    _impl.irFunction.fnDeclGenerator = fnDeclGenerator;
    _impl.irFunction.funcDeclNode = funcDecl;
}

AbstractFunction::AbstractFunction(NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes):
    _argsTypes(argTypes), _returnType(returnType){
    _native = true;
    _impl.nativeFunction = fn;
}

AbstractFunction::~AbstractFunction(){
}

bool AbstractFunction::isUsed() const {
    return _native || (_impl.irFunction.llvmFunction != NULL);
}

void AbstractFunction::setImplementationGenerator(std::function<void (llvm::Function*)> generator){
    if(_native){
        return;
    }
    _impl.irFunction.fnImplGenerator = generator;
    _impl.irFunction.funcDeclNode = NULL;
    if(isUsed()){
        _impl.irFunction.fnImplGenerator(_impl.irFunction.llvmFunction);
    }
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

AbstractFunction::operator llvm::Function*() {
    if(_native){
        return NULL;
    }
    if(_impl.irFunction.llvmFunction == NULL){
        _impl.irFunction.llvmFunction = _impl.irFunction.fnDeclGenerator();
        if(_impl.irFunction.fnImplGenerator){
            _impl.irFunction.fnImplGenerator(_impl.irFunction.llvmFunction);
        }
    }
    return _impl.irFunction.llvmFunction;
}

NativeFunction AbstractFunction::getNative() const {
    if(!_native){
        return NULL;
    }
    return _impl.nativeFunction;
}

const Type* AbstractFunction::getReturnType() const {
    return _returnType;
}

ast::Node *AbstractFunction::getDeclaration() const {
    if(_native){
        return NULL;
    }
    return _impl.irFunction.funcDeclNode;
}

} // namespace core
} // namespace sugar
