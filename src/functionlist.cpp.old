#include "functionlist.h"

#include "functionimpl.h"
#include <iostream>

namespace core {

FunctionList::FunctionList()
{
}

void FunctionList::addFunction(const std::string &name, llvm::Function *function, const std::list<llvm::Type *> &types){
    _functions.push_back(new FunctionImpl(name, function, types));
}

llvm::Function* FunctionList::getFunction(const std::string &name, const std::list<llvm::Type *> &types) const {
    for(std::list<FunctionImpl *>::const_iterator i = _functions.begin(); i != _functions.end(); i++){
        if((*i)->getName() == name && (*i)->match(types)){
           return (*i)->getLLVMFunction();
        }
    }
    return NULL;
}

FunctionList functionList;
}
