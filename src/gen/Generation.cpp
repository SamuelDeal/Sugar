#include "Generation.h"
#include <iostream>

namespace sugar {
namespace gen {

Generation::Generation() :
    boolType("Bool", llvm::IntegerType::get(context, 1)),
    intType("Int", llvm::Type::getInt64Ty(context)),
    floatType("Float", llvm::Type::getDoubleTy(context)),
    voidType("Void", llvm::Type::getVoidTy(context)),
    trueConst(llvm::ConstantInt::getTrue(context)),
    falseConst(llvm::ConstantInt::getFalse(context)),
    intZero(llvm::Constant::getNullValue(intType)),
    rootScope(NULL, NULL, core::ScopeType::Global),
    scope(&rootScope),
    builder(context)
{
    scope->addType(&boolType);
    scope->addType(&intType);
    scope->addType(&floatType);
    scope->addType(&voidType);
}

Generation::~Generation(){
    delete trueConst;
    delete falseConst;
    delete intZero;
}

llvm::Module* Generation::getModule() const {
    return module;
}

void Generation::pushBlock(llvm::BasicBlock *block, unsigned int scopeType) {
    core::Scope* oldScope = scope;
    scope = new core::Scope(block, oldScope, scopeType);
    if(scope->isFunction()){
        builder.SetInsertPoint(block);
    }
}

void Generation::popBlock() {
    core::Scope* oldScope = scope;
    scope = oldScope->getParent();
    if(oldScope->isFunction() && scope != NULL && !scope->isGlobal()){
        builder.SetInsertPoint(*scope);
    }
    delete oldScope;
}

core::Scope* Generation::getCurrentFunctionScope() {
    core::Scope* result = scope;
    while(result != NULL && !result->isFunction()){
        result = result->getParent();
    }
    if(result == NULL){
        std::cout << "this should not happen" << std::endl;
    }
    return result;
}

} // namespace gen
} // namespace sugar
