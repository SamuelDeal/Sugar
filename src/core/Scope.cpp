#include "Scope.h"

#include "../config_checked.h"
#include "Function.h"
#include "Operator.h"
#include "Variable.h"
#include "Type.h"

#include <sstream>
#include <iostream>
#include <llvm/BasicBlock.h>

namespace sugar {
namespace core {

int count = 0;

Scope::Scope(llvm::BasicBlock *block, Scope *parent, unsigned int type) {
    _parent = parent;
    _block = block;
    _type = type;
    ifCount = 0;
    loopCount = 0;
    _id = ++count;
    _returnReach = false;
#if DEBUG_GENERATOR
    std::cerr << "creating scope: " << this->toString() << std::endl;
#endif

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

bool Scope::isVarOwner() const {
    return ((_type & ScopeType::Global) == ScopeType::Global) ||
            ((_type & ScopeType::Function) == ScopeType::Function) ||
            ((_type & ScopeType::Protected) == ScopeType::Protected);
}

void Scope::addFunction(Function *function){
    _functions.insert(std::pair<const std::string, Function*>(function->getName(), function));
}

void Scope::addOperator(Operator *op){
    _operators.insert(std::pair<int, Operator *>(op->getOperatorId(), op));
}

void Scope::addVar(Variable *var){
    _variables.insert(std::pair<const std::string, Variable*>(var->getName(), var));
}

void Scope::addType(Type *type){
    _types.insert(std::pair<const std::string, Type*>(type->getName(), type));
}

std::list<Function*> Scope::getFuncs(const std::string &name) const {
    typedef std::multimap<const std::string, Function*>::const_iterator FuncIt;

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

std::list<Operator*> Scope::getOps(int operatorToken, bool before) const {
    typedef std::multimap<int, Operator*>::const_iterator OpIt;

    std::list<Operator *> results;
    std::pair<OpIt, OpIt> local = _operators.equal_range(operatorToken);
    for(OpIt it = local.first; it != local.second; it++){
        if(it->second->getBefore() == before){
            results.push_back(it->second);
        }
    }

    if(_parent != NULL){
        std::list<Operator*> previous = _parent->getOps(operatorToken, before);
        results.insert(results.end(), previous.begin(), previous.end());
    }
    return results;

}

Variable* Scope::getVar(const std::string &name) const {
    std::map<const std::string, Variable *>::const_iterator found = _variables.find(name);
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
    std::map<const std::string, Type *>::const_iterator found = _types.find(name);
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

core::Scope* Scope::getCurrentFunctionScope() {
    core::Scope* result = this;
    while(result != NULL && !result->isFunction()){
        result = result->getParent();
    }
    return result;
}

core::Scope* Scope::getCurrentVarOwnerScope() {
    core::Scope* result = this;
    while(result != NULL && !result->isVarOwner()){
        result = result->getParent();
    }
    return result;
}

void Scope::replaceBlock(llvm::BasicBlock *block){
    _block = block;
}

std::string Scope::toString(){
    std::stringstream out;
    out << (_block == NULL ? "NULL" : _block->getName().str()) << " (id: " << _id << ", type: " << std::hex << _type << ")";
    return out.str();
}

void Scope::setReturnReach(){
    _returnReach = true;
}

bool Scope::isReturnReach() {
    return _returnReach;
}

} // namespace core
} // namespace sugar
