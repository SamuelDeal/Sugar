#include "functionimpl.h"

#include <llvm/Type.h>
#include <iostream>

namespace core {


FunctionImpl::FunctionImpl(const std::string &name, llvm::Function *fn, const std::list<llvm::Type *> &types) :
    _name(name), _types(types), _function(fn)
{
}

bool FunctionImpl::match(const std::list<llvm::Type *> &types) const {
    std::list<llvm::Type *>::const_iterator mine = _types.begin();
    std::list<llvm::Type *>::const_iterator asked = types.begin();

    while(asked != types.end()){
        if(mine == _types.end()){
            return false;
        }
        if(*mine != *asked){
            return false;
        }
        ++asked;
        ++mine;
    }
    return mine == _types.end();
}

const std::string& FunctionImpl::getName() const {
    return _name;
}

llvm::Function* FunctionImpl::getLLVMFunction() const{
    return _function;
}

}
