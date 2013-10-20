#include "Scope.h"

#include "Function.h"
#include "Operator.h"
#include "Variable.h"
#include "Type.h"

#include <iostream>

namespace sugar {
namespace core {

Scope::Scope(llvm::BasicBlock *block, Scope *parent, unsigned int type) {
    _parent = parent;
    _block = block;
    _type = type;
    ifCount = 0;
}

Scope::~Scope(){
}

Scope* Scope::getParent() const {
    return _parent;
}

bool Scope::isGlobal() const {
    return _parent == NULL || ((_type & ScopeType::Global) == ScopeType::Global);
}

bool Scope::isFunction() const {
    return (_type & ScopeType::Function) == ScopeType::Function;
}

void Scope::addFunction(Function *function){
    _functions.insert(std::pair<std::string, Function*>(function->getName(), function));
}

void Scope::addOperator(Operator *op){
    _operators.insert(std::pair<int, Operator *>(op->getOperatorId(), op));
}

void Scope::addVar(Variable *var){
    _variables.insert(std::pair<std::string, Variable*>(var->getName(), var));
}

void Scope::addType(Type *type){
    _types.insert(std::pair<std::string, Type*>(type->getName(), type));
}

std::list<Function*> Scope::getFuncs(const std::string &name) const {
    typedef std::multimap<std::string, Function*>::const_iterator FuncIt;

    std::list<Function *> results;
    std::pair<FuncIt, FuncIt> local = _functions.equal_range(name);
    for(FuncIt it = local.first; it != local.second; it++){
        results.push_back(it->second);
    }

    if(_parent != NULL){
        std::list<Function*> previous = _parent->getFuncs(name);
        results.insert(results.end(), previous.begin(), previous.end());
    }
    return results;
}

std::list<Operator*> Scope::getOps(int operatorToken) const {
    typedef std::multimap<int, Operator*>::const_iterator OpIt;

    std::list<Operator *> results;
    std::pair<OpIt, OpIt> local = _operators.equal_range(operatorToken);
    for(OpIt it = local.first; it != local.second; it++){
        results.push_back(it->second);
    }

    if(_parent != NULL){
        std::list<Operator*> previous = _parent->getOps(operatorToken);
        results.insert(results.end(), previous.begin(), previous.end());
    }
    return results;

}

Variable* Scope::getVar(const std::string &name) const {
    std::map<std::string, Variable *>::const_iterator found = _variables.find(name);
    if(found != _variables.end()){
        return found->second;
    }
    else if(_parent != NULL){
        return _parent->getVar(name);
    }
    else{
        return NULL;
    }
}

Type* Scope::getType(const std::string &name) const {
    std::map<std::string, Type *>::const_iterator found = _types.find(name);
    if(found != _types.end()){
        return found->second;
    }
    else if(_parent != NULL){
        return _parent->getType(name);
    }
    else{
        return NULL;
    }
}

Scope::operator llvm::BasicBlock*() const {
    return _block;
}

} // namespace core
} // namespace sugar
