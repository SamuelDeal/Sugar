#include "Generation.h"

#include "../utils/config_checked.h"

#include <iostream>
#include "../utils/Getter.tpl.h"
#include "../parser/parser.hpp"

void yyerror(YYLTYPE *locp, sugar::parser::LexerContext *lexerCtx, const char *err);

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
    builder(context),
    useColor(false)
{
    scope->addType(&boolType);
    scope->addType(&intType);
    scope->addType(&floatType);
    scope->addType(&voidType);
    _errorCount = 0;
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
#if DEBUG_GENERATOR
    std::cerr << "adding scope: " << scope->toString() << std::endl;
#endif
#if DEBUG_GENERATOR
    std::cerr << "auto define scope as insertion point: " << scope->toString() << std::endl;
#endif
    builder.SetInsertPoint(block);
#if DEBUG_GENERATOR
    std::cerr << scopeHierarchy() << std::endl;
#endif
}

void Generation::popBlock() {
    core::Scope* oldScope = scope;
    scope = oldScope->getParent();
#if DEBUG_GENERATOR
    std::cerr << "removing scope point: " << oldScope->toString() << std::endl;
#endif
    if(scope != NULL && !scope->isGlobal()){
#if DEBUG_GENERATOR
        std::cerr << "auto define scope as insertion point: " << scope->toString() << std::endl;
#endif
        builder.SetInsertPoint(*scope);
    }
    delete oldScope;
}

std::string Generation::scopeHierarchy() {
    std::string result = "";
    std::string indent = "";
    core::Scope *current = scope;
    while(current != NULL){
        result += indent + "->" + current->toString() + "\n";
        indent += "  ";
        current = current->getParent();
    }
    return result;
}

llvm::Function* Generation::getInternalFunction(const std::string &name){
    std::map<const std::string, utils::Getter<llvm::Function*> >::iterator it = _internalFunctions.find(name);
    if(it == _internalFunctions.end()){
        return NULL;
    }
    return it->second;
}

void Generation::addInternalFunction(const std::string &name, utils::Getter<llvm::Function*> getter){
    _internalFunctions.insert(std::make_pair(name, getter));
}

void Generation::addError(const std::string &error, YYLTYPE *location){
    if(!maxErrorCountReached()){
        yyerror(location, NULL, error.c_str());
    }
    ++_errorCount;
}

bool Generation::maxErrorCountReached() const {
    return _errorCount > MAX_ERROR_COUNT;
}

} // namespace gen
} // namespace sugar
