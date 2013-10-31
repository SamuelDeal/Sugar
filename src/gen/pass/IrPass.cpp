#include "IrPass.h"

#include <iostream>

#include "../../config_checked.h"
#include "../../ast/all.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/Instructions.h>
#include <llvm/InstrTypes.h>
#include <llvm/GlobalVariable.h>

#include "../../core/Type.h"
#include "../../core/Function.h"
#include "../../core/Variable.h"
#include "../../core/Operator.h"
#include "../../core/Cast.h"
#include "../../parser/parser.hpp"

#include "../GeneratedCode.h"
#include "../Generation.h"

namespace sugar {
namespace gen {
namespace pass {

using namespace core;

IrPass::IrPass()
{
}

llvm::Value* IrPass::parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen) {
    //Nothing to do
    return NULL;
}

llvm::Value* IrPass::parse(ast::Node *node, ast::Assignment *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::cerr << "Creating assignment for " << *data->getLeft()->name << std::endl;
#endif
    core::Variable *var = gen.scope->getVar(*data->getLeft()->name);
    if(var == NULL){
        std::cout << "undeclared variable " << *data->getLeft()->name << std::endl;
        return NULL;
    }
    llvm::Value *value = parseNode(data->right, gen);
    if(value == NULL){
        return NULL;
    }
    if(*data->right->getType() != var->getType()){
        std::cout << "Uncompatible types in assigment  " << data->right->getType()->getName()
                  << " to " << var->getType().getName() << std::endl;
        return NULL;
    }
    llvm::StoreInst *assign = gen.builder.CreateStore(value, *var);
    return assign;
}

llvm::Value* IrPass::parse(ast::Node *node, ast::Block *data, Generation &gen) {
    std::list<ast::Node*>::const_iterator it;
    llvm::Value *lastValue = NULL;
    ast::Node *lastStatement = NULL;
    for (it = data->stmts.begin(); it != data->stmts.end() && !gen.scope->isReturnReach(); it++) {
#if DEBUG_GENERATOR
        std::cerr << "Generating code for statement" << std::endl;
#endif
        lastStatement = *it;
        lastValue = parseNode(lastStatement, gen);
    }
#if DEBUG_GENERATOR
    std::cerr << "Creating block" << std::endl;
#endif
    if(lastStatement == NULL){
        node->setType(gen.voidType);
    }
    else{
        node->setType(*lastStatement->getType());
    }
    return lastValue;
}

llvm::Value* IrPass::parse(ast::Node *node, ast::Constant *data, Generation &gen) {
    switch(data->getConstantType()){
        case ast::Constant::eBoolean:
            node->setType(gen.boolType);
            return llvm::ConstantInt::get(gen.boolType, data->value.boolValue == false ? 0 : 1, false);
        case ast::Constant::eInteger:
            node->setType(gen.intType);
            return llvm::ConstantInt::get(gen.intType, data->value.integerValue, true);
        case ast::Constant::eFloat:
            node->setType(gen.floatType);
            return llvm::ConstantFP::get(gen.floatType, data->value.floatValue);
        default:
            std::cout << "Unknown constant type" << std::endl;
            return NULL;
    }
}

llvm::Value* IrPass::parse(ast::Node *node, ast::FunctionCall *data, Generation &gen) {
    std::string *functionName = data->getFunctionName()->name;
#if DEBUG_GENERATOR
    std::cerr << "\n*** Method call generation " << *functionName << std::endl;
#endif
    std::vector<llvm::Value*> args;
    std::list<ast::Node*>::const_iterator it;
    std::list<const Type *> types;
    for (it = data->args->begin(); it != data->args->end(); it++) {
        args.push_back(parseNode(*it, gen));
        const Type* argType = (*it)->getType();
        if(argType == NULL){
            std::cout << "Parsing failed " << std::endl;
            return NULL;
        }
        else {
            types.push_back(argType);
        }
    }

    std::list<Function*> functions = gen.scope->getFuncs(*functionName);

    if (functions.empty()){
        std::cout << "undeclared function " << *functionName << std::endl;
        return NULL;
    }
    else{
        for(std::list<Function *>::iterator it = functions.begin(); it != functions.end(); it++){
            Function *func = *it;
            if(func->match(types, gen.castGraph)){
                node->setType(*func->getReturnType());
                //FIXME: match & casts
                llvm::Value *call;
                if(func->isNative()){
                    call = func->getNative()(args, gen);
                }
                else{
                    call = gen.builder.CreateCall(*func, makeArrayRef(args));
                }
#if DEBUG_GENERATOR
                std::cerr << "Creating method call " << std::endl;
#endif
                return call;
            }
        }
        std::cout << "no function match given arguments" << std::endl;
        return NULL;
    }
}

llvm::Value* IrPass::parse(ast::Node *node, ast::Operator *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::string debugTypes;
#endif
    std::vector<llvm::Value*> args;
    std::list<ast::Node*>::const_iterator it;
    std::list<const Type *> types;
    for (it = data->args->begin(); it != data->args->end(); it++) {
        args.push_back(parseNode(*it, gen));
        types.push_back((*it)->getType());
#if DEBUG_GENERATOR
        debugTypes += " " + (*it)->getType()->getName();
#endif
    }

#if DEBUG_GENERATOR
    std::cerr << "\n*** Operator call generation: " << data->operatorId << debugTypes << std::endl;
#endif
    std::list<Operator*> operators = gen.scope->getOps(data->operatorId);

    if (operators.empty()){
        std::cout << "undeclared operator" << std::endl;
        return NULL;
    }
    else{
        for(std::list<Operator *>::iterator it = operators.begin(); it != operators.end(); it++){
            if((*it)->match(types, gen.castGraph)){
                node->setType(*(*it)->getReturnType());
                //FIXME: match & casts
                llvm::Value *call;
                if((*it)->isNative()){
                    call = (*it)->getNative()(args, gen);
                }
                else{
                    call = gen.builder.CreateCall(**it, makeArrayRef(args));
                }
#if DEBUG_GENERATOR
                std::cerr << "Creating operator call " << std::endl;
#endif
                return call;
            }
        }
        std::cout << "no operator match given arguments : " <<  data->operatorId << std::endl;
        return NULL;
    }
}


llvm::Value* IrPass::parse(ast::Node *node, ast::Identifier *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::cerr << "\n*** Identifier " << *data->name << std::endl;
#endif
    Variable *var = gen.scope->getVar(*data->name);
    if(var != NULL){
        node->setType(var->getType());
        return new llvm::LoadInst(*var, "", false, *gen.scope);
    }
    else{
        std::list<Function *> functions = gen.scope->getFuncs(*data->name);
        if (functions.empty()){
            std::cout << "undeclared variable or function " << *data->name << std::endl;
            return NULL;
        }
        else{
            std::list<const Type *> types;
            for(std::list<Function *>::iterator it = functions.begin(); it != functions.end(); it++){
                if((*it)->match(types, gen.castGraph)){
                    node->setType(*(*it)->getReturnType());
                    llvm::Value *call;
                    if((*it)->isNative()){
                        std::vector<llvm::Value*> args;
                        call = (*it)->getNative()(args, gen);
                    }
                    else{
                        call = gen.builder.CreateCall(**it);
                    }
#if DEBUG_GENERATOR
                    std::cerr << "Creating method call " << *data->name << std::endl;
#endif
                    return call;
                }
            }
            std::cout << "undeclared variable or function " << *data->name << std::endl;
            return NULL;
        }
    }
}

llvm::Value* IrPass::parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen) {
    Type *returnType = gen.scope->getType(*data->getType()->name);
    if(NULL == returnType){
        std::cout << "Unknown type " << *data->getType()->name << std::endl;
        return NULL;
    }

    std::vector<llvm::Type*> argTypes;
    std::list<ast::Node*>::const_iterator it;
    for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
        Type *argType = gen.scope->getType(*((ast::ArgumentDeclaration*)(*it)->data)->getType()->name);
        if(argType == NULL){
            std::cout << "Unknown type " << *((ast::ArgumentDeclaration*)(*it)->data)->getType()->name << std::endl;
            return NULL;
        }
        argTypes.push_back(*argType);
    }
    llvm::FunctionType *ftype = llvm::FunctionType::get(*returnType, llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, data->getId()->name->c_str(), gen.module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", function, 0);

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;
    std::list<const Type *> types;

    gen.pushBlock(bblock, ScopeType::Function);

    for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
        parseNode(*it, gen);
        argumentValue = argsValues++;
        std::string *argumentName = ((ast::ArgumentDeclaration*)((*it)->data))->getId()->name;
        argumentValue->setName(argumentName->c_str());
        gen.builder.CreateStore(argumentValue, *gen.scope->getVar(*argumentName));
        types.push_back((*it)->getType());
    }

    llvm::Value *blockReturnValue = parseNode(data->block, gen);
    if(data->block->getType() != returnType){
        std::cout << "Function type mismatch with declaration !"  << std::endl;
        return NULL;
    }

    if(*returnType != gen.voidType){
        gen.builder.CreateRet(blockReturnValue);
    }
    else{
        gen.builder.CreateRetVoid();
    }

    gen.popBlock();
    gen.scope->addFunction(new Function(*data->getId()->name, function, gen.scope->getType(*data->getType()->name), types));
#if DEBUG_GENERATOR
    std::cerr << "Creating function: " << *data->getId()->name << std::endl;
#endif
    return function;
}

llvm::Value* IrPass::parse(ast::Node *node, ast::ArgumentDeclaration *data, Generation &gen) {
    Type *type = gen.scope->getType(*data->getType()->name);
    if(type == NULL){
        std::cout << "Unknown type " << *data->getType()->name << std::endl;
        return NULL;
    }
    node->setType(*type);
    Scope *scope = gen.scope->getCurrentVarOwnerScope();
#if DEBUG_GENERATOR
    std::cerr << "Creating argument declaration " << *data->getType()->name << " " << *data->getId()->name << std::endl;
#endif
    llvm::AllocaInst *alloc = new llvm::AllocaInst(*type, data->getId()->name->c_str(), *scope);
    scope->addVar(new Variable(*data->getId()->name, *type, alloc));
    if(data->defaultValue != NULL){
#if DEBUG_GENERATOR
        std::cerr << "Assigning value to argument declaration " << type->getName() << " " << *data->getId()->name << std::endl;
#endif
        return parseNode(ast::Assignment::create(data->id, data->defaultValue), gen);
    }
    return alloc;
}


llvm::Value* IrPass::parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen) {
    Type *type = gen.scope->getType(*data->getType()->name);
    if(type == NULL){
        std::cout << "Unknown type " << *data->getType()->name << std::endl;
        return NULL;
    }
    node->setType(*type);
    Scope *scope = gen.scope->getCurrentVarOwnerScope();
    if(scope->isGlobal()){
#if DEBUG_GENERATOR
        std::cerr << "Creating global variable declaration " << *data->getType()->name << " " << *data->getId()->name << std::endl;
#endif
        llvm::GlobalVariable *var = new llvm::GlobalVariable(*gen.module, *type, false,
            llvm::GlobalValue::InternalLinkage, 0, *data->getId()->name);
        var->setInitializer(llvm::Constant::getNullValue(*type));
        scope->addVar(new Variable(*data->getId()->name, *type, var));
        if(data->assign != NULL){
#if DEBUG_GENERATOR
            std::cerr << "Assigning value to global variable declaration " << type->getName() << " " << data->getId()->name << std::endl;
#endif
            return parseNode(ast::Assignment::create(data->id, data->assign), gen);
        }
        //lastGlobal = var;
        return var;
    }
    else { //local variable declaration
#if DEBUG_GENERATOR
        std::cerr << "Creating local variable declaration " << *data->getType()->name << " " << *data->getId()->name << std::endl;
#endif
        llvm::AllocaInst *alloc = new llvm::AllocaInst(*type, data->getId()->name->c_str(), *scope);
        scope->addVar(new Variable(*data->getId()->name, *type, alloc));
        if(data->assign != NULL){
#if DEBUG_GENERATOR
            std::cerr << "Assigning value to local variable declaration " << type->getName() << " " << *data->getId()->name << std::endl;
#endif
            return parseNode(ast::Assignment::create(data->id, data->assign), gen);
        }
        return alloc;
    }
}

llvm::Value* IrPass::parse(ast::Node *node, ast::IfExpression *data, Generation &gen) {
    llvm::Value *condV = parseNode(data->cond, gen);

    llvm::Function *currentFunction = gen.builder.GetInsertBlock()->getParent();
    core::Scope *currentFunctionScope = gen.scope->getCurrentFunctionScope();
    unsigned int ifCount = ++currentFunctionScope->ifCount;
    bool phi = true;
    std::string ifIndex = "";
    if(ifCount > 1){
        ifIndex = std::to_string(ifCount);
    }
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(gen.context, "if"+ifIndex, currentFunction);
    llvm::BasicBlock *elseBB = NULL;
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(gen.context, "endif"+ifIndex);
    if(data->elseBlk != NULL){
        elseBB = llvm::BasicBlock::Create(gen.context, "else"+ifIndex);
        gen.builder.CreateCondBr(condV, thenBB, elseBB);
    }
    else{
        gen.builder.CreateCondBr(condV, thenBB, mergeBB);
        phi = false;
    }

    gen.pushBlock(thenBB);
    llvm::Value *thenValue = parseNode(data->thenBlk, gen);
    if(gen.scope->isReturnReach()){
        phi = false;
    }
    else{
        gen.builder.CreateBr(mergeBB);
    }
    thenBB = gen.builder.GetInsertBlock();
    gen.popBlock();

    llvm::Value *elseValue = NULL;
    if(data->elseBlk != NULL){
        currentFunction->getBasicBlockList().push_back(elseBB);
        gen.pushBlock(elseBB);
        elseValue = parseNode(data->elseBlk, gen);
        gen.builder.CreateBr(mergeBB);
        elseBB = gen.builder.GetInsertBlock();
        gen.popBlock();
    }

    currentFunction->getBasicBlockList().push_back(mergeBB);
    gen.pushBlock(mergeBB);
    gen.scope->getParent()->replaceBlock(mergeBB);
    if(!phi || data->elseBlk->getType() != data->thenBlk->getType() ||
            *data->elseBlk->getType() == gen.voidType){
        phi = false;
    }

    if(!phi){
#if DEBUG_GENERATOR
        std::cerr << "skip phi value" << std::endl;
#endif
        node->setType(gen.voidType);
        gen.popBlock();
#if DEBUG_GENERATOR
        std::cerr << gen.scopeHierarchy() << std::endl;
#endif
        return NULL;
    }
    else{
#if DEBUG_GENERATOR
        std::cerr << "generating phi value" << std::endl;
#endif
        node->setType(*data->elseBlk->getType());
        llvm::PHINode *phiValue = gen.builder.CreatePHI(*node->getType(), 2);
        phiValue->addIncoming(thenValue, thenBB);
        phiValue->addIncoming(elseValue, elseBB);
        gen.popBlock();
#if DEBUG_GENERATOR
        std::cerr << gen.scopeHierarchy() << std::endl;
#endif
        return phiValue;
    }
}

llvm::Value* IrPass::parse(ast::Node *node, ast::Comparison *data, Generation &gen) {
    node->setType(gen.boolType);

    std::list<ast::Node *>::iterator it = data->expressions.begin();
    std::list<int>::iterator opIt = data->operators.begin();
    ast::Node *left = *it;
    llvm::Value *result = NULL;
    llvm::Value *valueComp = NULL;

    for(++it; it != data->expressions.end(); it++){
        std::list<ast::Node*> *args = new std::list<ast::Node*>();
        args->push_back(left);
        args->push_back(*it);
        ast::Node *operatorNode = ast::Operator::create(*opIt, args);
        valueComp = parseNode(operatorNode, gen);
        if(valueComp == NULL){
            std::cout << "comparison failed" << std::endl;
            return NULL;
        }
        if(*operatorNode->getType() != gen.boolType){
            std::cout << "comparison is not boolean" << std::endl;
            return NULL;
        }
        ((ast::Operator*)(operatorNode->data))->args->clear();
        delete operatorNode;
        left = *it;
        if(result == NULL){
            result = valueComp;
        }
        else{
            result = gen.builder.CreateAnd(result, valueComp);
        }
        ++opIt;
    }
    return result;
}

llvm::Value* IrPass::parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen) {
    llvm::Value* expr = parseNode(data->expression, gen);
    llvm::Value* returnExpr = gen.builder.CreateRet(expr);
    gen.scope->setReturnReach();
    return returnExpr;
}

llvm::Value* IrPass::parse(ast::Node *node, ast::WhileStmt *data, Generation &gen) {
    node->setType(gen.voidType);

    llvm::Function *currentFunction = gen.builder.GetInsertBlock()->getParent();
    core::Scope *currentFunctionScope = gen.scope->getCurrentFunctionScope();
    unsigned int loopCount = ++currentFunctionScope->loopCount;
    std::string loopIndex = "";
    if(loopCount > 1){
        loopIndex = std::to_string(loopCount);
    }

    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(gen.context, "loopcond"+loopIndex);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(gen.context, "loop"+loopIndex);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(gen.context, "endloop"+loopIndex);

    gen.builder.CreateBr(data->testFirst ? condBB : loopBB);
    currentFunction->getBasicBlockList().push_back(condBB);
    gen.pushBlock(condBB);
    llvm::Value *condV = parseNode(data->cond, gen);
    gen.builder.CreateCondBr(condV, loopBB, mergeBB);
    gen.popBlock();

    currentFunction->getBasicBlockList().push_back(loopBB);
    gen.pushBlock(loopBB);
    parseNode(data->block, gen);
    if(!gen.scope->isReturnReach()){
        gen.builder.CreateBr(condBB);
    }
    loopBB = gen.builder.GetInsertBlock();
    gen.popBlock();

    currentFunction->getBasicBlockList().push_back(mergeBB);
    gen.pushBlock(mergeBB);
    gen.scope->getParent()->replaceBlock(mergeBB);
    gen.popBlock();

    return NULL;
}

} // namespace pass
} // namespace gen
} // namespace sugar
