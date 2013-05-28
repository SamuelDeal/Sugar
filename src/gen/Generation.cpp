#include "Generation.h"

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
    rootScope(NULL, NULL),
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

void Generation::pushBlock(llvm::BasicBlock *block, bool forceGlobal) {
    core::Scope* oldScope = scope;
    scope = new core::Scope(block, oldScope, forceGlobal);
    builder.SetInsertPoint(block);
}

void Generation::popBlock() {
    core::Scope* oldScope = scope;
    scope = oldScope->getParent();
    delete oldScope;
    if(scope != NULL && !scope->isGlobal()){
        builder.SetInsertPoint(*scope);
    }
}

} // namespace gen
} // namespace sugar
