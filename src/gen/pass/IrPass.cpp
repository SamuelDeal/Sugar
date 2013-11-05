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
#include "../../utils/Getter.tpl.h"

namespace sugar {
namespace gen {
namespace pass {

using namespace core;

IrPass::IrPass()
{
}

void IrPass::parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen) {
    //Nothing to do
    return;
}

void IrPass::parse(ast::Node *node, ast::Assignment *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::cerr << "Creating assignment" << std::endl;
#endif
    parseNode(data->left, gen);
    /*core::Variable *var = gen.scope->getVar(*data->getLeft()->name);
    if(var == NULL){
        std::cout << "undeclared variable " << *data->getLeft()->name << std::endl;
        return NULL;
    }*/
    llvm::Value* leftRef = data->left->getRef();
    if(leftRef == NULL){
        std::cout << "Left part is not assignable" << std::endl;
    }
    parseNode(data->right, gen);
    if(*data->right->getType() != *data->left->getType()){
        std::cout << "Uncompatible types in assigment  " << data->right->getType()->getName()
                  << " to " << data->left->getType()->getName() << std::endl;
        return;
    }
    gen.builder.CreateStore(data->right->getValue(), leftRef);
    node->setType(gen.voidType);
}

void IrPass::parse(ast::Node *node, ast::Block *data, Generation &gen) {
    std::list<ast::Node*>::const_iterator it;
    llvm::Value *lastValue = NULL;
    ast::Node *lastStatement = NULL;
    for (it = data->stmts->begin(); it != data->stmts->end() && !gen.scope->isReturnReach(); it++) {
#if DEBUG_GENERATOR
        std::cerr << "Generating code for statement" << std::endl;
#endif
        lastStatement = *it;
        parseNode(lastStatement, gen);
        lastValue = (*it)->getValue();
    }
#if DEBUG_GENERATOR
    std::cerr << "Creating block" << std::endl;
#endif
    if(lastValue == NULL || lastStatement == NULL || lastStatement->getType() == &gen.voidType){
        node->setType(gen.voidType);
    }
    else{
        node->setType(*lastStatement->getType());
        node->setValue(lastValue);
    }
}

void IrPass::parse(ast::Node *node, ast::FunctionImplementation *data, Generation &gen) {
    std::list<ast::Node*>::const_iterator it;
    llvm::Value *lastValue = NULL;
    ast::Node *lastStatement = NULL;
    for (it = data->stmts->begin(); it != data->stmts->end() && !gen.scope->isReturnReach(); it++) {
#if DEBUG_GENERATOR
        std::cerr << "Generating code for statement" << std::endl;
#endif
        lastStatement = *it;
        parseNode(lastStatement, gen);
        lastValue = lastStatement->getValue();
    }
#if DEBUG_GENERATOR
    std::cerr << "Creating block" << std::endl;
#endif
    if(lastValue == NULL || lastStatement == NULL || lastStatement->getType() == &gen.voidType){
        node->setType(gen.voidType);
    }
    else{
        node->setType(*lastStatement->getType());
        node->setValue(lastValue);
    }
}

void IrPass::parse(ast::Node *node, ast::Constant *data, Generation &gen) {
    switch(data->getConstantType()){
        case ast::Constant::eBoolean:
            node->setType(gen.boolType);
            node->setValue(llvm::ConstantInt::get(gen.boolType, data->value.boolValue == false ? 0 : 1, false));
            break;
        case ast::Constant::eInteger:
            node->setType(gen.intType);
            node->setValue(llvm::ConstantInt::get(gen.intType, data->value.integerValue, true));
            break;
        case ast::Constant::eFloat:
            node->setType(gen.floatType);
            node->setValue(llvm::ConstantFP::get(gen.floatType, data->value.floatValue));
            break;
        default:
            std::cout << "Unknown constant type" << std::endl;
            return;
    }
}

void IrPass::parse(ast::Node *node, ast::FunctionCall *data, Generation &gen) {
    std::string *functionName = data->getFunctionName()->name;
#if DEBUG_GENERATOR
    std::cerr << "\n*** Method call generation " << *functionName << std::endl;
#endif



    std::vector<llvm::Value*> args;
    std::vector<ast::Node*> nodeArgs;
    std::list<ast::Node*>::const_iterator it;
    std::list<const Type *> types;
    for (it = data->args->begin(); it != data->args->end(); it++) {
        parseNode(*it, gen);
        nodeArgs.push_back(*it);
        args.push_back((*it)->getValue());
        const Type* argType = (*it)->getType();
        if(argType == NULL){
            std::cout << "Parsing failed " << std::endl;
            return;
        }
        else {
            types.push_back(argType);
        }
    }

    std::list<Function*> functions = gen.scope->getFuncs(*functionName);

    if (functions.empty()){
        std::cout << "undeclared function " << *functionName << std::endl;
        return;
    }
    else{
        for(std::list<Function *>::iterator it = functions.begin(); it != functions.end(); it++){
            Function *func = *it;
            if(func->match(types, gen.castGraph)){
                node->setType(*func->getReturnType());
                //FIXME: match & casts
                llvm::Value *call;
                if(func->isNative()){
                    call = func->getNative()(nodeArgs, gen);
                }
                else{
                    call = gen.builder.CreateCall(*func, makeArrayRef(args));
                }
#if DEBUG_GENERATOR
                std::cerr << "Creating method call " << std::endl;
#endif
                node->setValue(call);
                return;
            }
        }
        std::cout << "no function match given arguments" << std::endl;
        return;
    }
}

void IrPass::parse(ast::Node *node, ast::Operator *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::string debugTypes;
#endif
    std::vector<llvm::Value*> args;
    std::vector<ast::Node*> nodeArgs;
    std::list<ast::Node*>::const_iterator it;
    std::list<const Type *> types;
    for (it = data->args->begin(); it != data->args->end(); it++) {
        parseNode(*it, gen);
        nodeArgs.push_back(*it);
        args.push_back((*it)->getValue());
        types.push_back((*it)->getType());
#if DEBUG_GENERATOR
        debugTypes += " " + (*it)->getType()->getName();
#endif
    }

#if DEBUG_GENERATOR
    std::cerr << "\n*** Operator call generation: " << data->operatorId << debugTypes << std::endl;
#endif
    std::list<Operator*> operators = gen.scope->getOps(data->operatorId, data->before);

    if (operators.empty()){
        std::cout << "undeclared operator" << std::endl;
        return;
    }
    else{
        for(std::list<Operator *>::iterator it = operators.begin(); it != operators.end(); it++){
            if((*it)->match(types, gen.castGraph)){
                node->setType(*(*it)->getReturnType());
                //FIXME: match & casts
                llvm::Value *call;
                if((*it)->isNative()){
                    call = (*it)->getNative()(nodeArgs, gen);
                }
                else{
                    call = gen.builder.CreateCall(**it, makeArrayRef(args));
                }
#if DEBUG_GENERATOR
                std::cerr << "Creating operator call " << std::endl;
#endif
                node->setValue(call);
                return;
            }
        }
        std::cout << "no operator match given arguments : " <<  data->operatorId << std::endl;
        return;
    }
}


void IrPass::parse(ast::Node *node, ast::Identifier *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::cerr << "\n*** Identifier " << *data->name << std::endl;
#endif
    Variable *var = gen.scope->getVar(*data->name);
    if(var != NULL){
        node->setType(var->getType());
        core::Scope *scope = gen.scope;

        std::cerr << "Getting var data 1 " << std::endl;
        std::cerr << var << ", " << *var << std::endl;
        std::cerr << scope << ", " << *scope << std::endl;

        node->setValue(utils::Getter<llvm::Value*>([=] {
            std::cerr << "Getting var data 2 " << std::endl;
            std::cerr << var << ", " << *var << std::endl;
            std::cerr << scope << ", " << *scope << std::endl;
            //std::cerr << var->getName() << ", " << var->toString() << std::endl;
            return new llvm::LoadInst(*var, "", false, *scope);
        }));
        node->setRef(*var);
    }
    else{
        std::list<Function *> functions = gen.scope->getFuncs(*data->name);
        if (functions.empty()){
            std::cout << "undeclared variable or function " << *data->name << std::endl;
            return;
        }
        else{
            std::list<const Type *> types;
            for(std::list<Function *>::iterator it = functions.begin(); it != functions.end(); it++){
                if((*it)->match(types, gen.castGraph)){
                    node->setType(*(*it)->getReturnType());
                    llvm::Value *call;
                    if((*it)->isNative()){
                        std::vector<ast::Node*> args;
                        call = (*it)->getNative()(args, gen);
                    }
                    else{
                        call = gen.builder.CreateCall(**it);
                    }
#if DEBUG_GENERATOR
                    std::cerr << "Creating method call " << *data->name << std::endl;
#endif
                    node->setValue(call);
                    return;
                }
            }
            std::cout << "undeclared variable or function " << *data->name << std::endl;
            return;
        }
    }
}

void IrPass::parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen) {
    Type *returnType = gen.scope->getType(*data->getType()->name);
    if(NULL == returnType){
        std::cout << "Unknown type " << *data->getType()->name << std::endl;
        return;
    }

    std::list<ast::Node*>::const_iterator it;
    std::list<const Type*> types;
    for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
        Type *argType = gen.scope->getType(*((ast::ArgumentDeclaration*)(*it)->data)->getType()->name);
        if(argType == NULL){
            std::cout << "Unknown type " << *((ast::ArgumentDeclaration*)(*it)->data)->getType()->name << std::endl;
            return;
        }
        types.push_back(argType);
    }


    Function *fn = new Function(*data->getId()->name, gen.scope->getType(*data->getType()->name), types, [=, &gen] () -> llvm::Function* {
        Type *returnType = gen.scope->getType(*data->getType()->name);
        std::vector<llvm::Type*> argTypes;
        std::list<ast::Node*>::const_iterator it;
        for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
            Type *argType = gen.scope->getType(*((ast::ArgumentDeclaration*)(*it)->data)->getType()->name);
            argTypes.push_back(*argType);
        }

        llvm::FunctionType *ftype = llvm::FunctionType::get(*returnType, llvm::makeArrayRef(argTypes), false);
        llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, data->getId()->name->c_str(), gen.module);
        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", function, 0);

        llvm::Function::arg_iterator argsValues = function->arg_begin();
        llvm::Value* argumentValue;
        gen.pushBlock(bblock, ScopeType::Function);

        for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
            parseNode(*it, gen);
            argumentValue = argsValues++;
            std::string *argumentName = ((ast::ArgumentDeclaration*)((*it)->data))->getId()->name;
            argumentValue->setName(argumentName->c_str());
            gen.builder.CreateStore(argumentValue, *gen.scope->getVar(*argumentName));
        }

        parseNode(data->impl, gen);
        llvm::Value *blockReturnValue = data->impl->getValue();
        if(data->impl->getType() != returnType){
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
        return function;
    });
    gen.scope->addFunction(fn);
#if DEBUG_GENERATOR
    std::cerr << "Creating function: " << *data->getId()->name << std::endl;
#endif
    //node->setValue(function);
}

/*void IrPass::generateFunction(IrPass *that, ast::FunctionDeclaration *data, llvm::Function *function, Generation &gen) {

}*/

void IrPass::parse(ast::Node *node, ast::ArgumentDeclaration *data, Generation &gen) {
    Type *type = gen.scope->getType(*data->getType()->name);
    if(type == NULL){
        std::cout << "Unknown type " << *data->getType()->name << std::endl;
        return;
    }
    node->setType(*type);
    Scope *scope = gen.scope->getCurrentVarOwnerScope();
#if DEBUG_GENERATOR
    std::cerr << "Creating argument declaration " << *data->getType()->name << " " << *data->getId()->name << std::endl;
#endif
    llvm::AllocaInst *alloc = new llvm::AllocaInst(*type, data->getId()->name->c_str(), *scope);
    Variable *var = new Variable(*data->getId()->name, *type, alloc);
    scope->addVar(var);
/*    if(data->defaultValue != NULL){
#if DEBUG_GENERATOR
        std::cerr << "Assigning value to argument declaration " << type->getName() << " " << *data->getId()->name << std::endl;
#endif
        ast::Node *assign = ast::Assignment::create(data->id, data->defaultValue, node->yylloc);
        parseNode(assign, gen);
        assign->
    }*/
    node->setRef(*var);
    node->setValue(alloc);
    //return alloc;
}


void IrPass::parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen) {
    Type *type = gen.scope->getType(*data->getType()->name);
    if(type == NULL){
        std::cout << "Unknown type " << *data->getType()->name << std::endl;
        return;
    }
    node->setType(*type);
    Scope *scope = gen.scope->getCurrentVarOwnerScope();
    if(scope->isGlobal()){
#if DEBUG_GENERATOR
        std::cerr << "Creating global variable declaration " << *data->getType()->name << " " << *data->getId()->name << std::endl;
#endif
        llvm::GlobalVariable *globalVar = new llvm::GlobalVariable(*gen.module, *type, false,
            llvm::GlobalValue::InternalLinkage, 0, *data->getId()->name);
        Variable *var = new Variable(*data->getId()->name, *type, globalVar);
        std::cerr << "Setting global var data" << std::endl;
        std::cerr << var << ", " << *var << std::endl;
        std::cerr << scope << ", " << *scope << std::endl;
        globalVar->setInitializer(llvm::Constant::getNullValue(*type));
        scope->addVar(var);
/*        if(data->assign != NULL){
#if DEBUG_GENERATOR
            std::cerr << "Assigning value to global variable declaration " << type->getName() << " " << data->getId()->name << std::endl;
#endif
            return parseNode(ast::Assignment::create(data->id, data->assign, node->yylloc), gen);
        }*/
        node->setRef(*var);
        node->setValue(globalVar);
        //lastGlobal = var;
        //return var;
    }
    else { //local variable declaration
#if DEBUG_GENERATOR
        std::cerr << "Creating local variable declaration " << *data->getType()->name << " " << *data->getId()->name << std::endl;
#endif
        llvm::AllocaInst *alloc = new llvm::AllocaInst(*type, data->getId()->name->c_str(), *scope);
        Variable *var = new Variable(*data->getId()->name, *type, alloc);
        scope->addVar(var);
        std::cerr << "Setting local var data" << std::endl;
        std::cerr << var << ", " << *var << std::endl;
        std::cerr << scope << ", " << *scope << std::endl;

/*        if(data->assign != NULL){
#if DEBUG_GENERATOR
            std::cerr << "Assigning value to local variable declaration " << type->getName() << " " << *data->getId()->name << std::endl;
#endif
            return parseNode(ast::Assignment::create(data->id, data->assign, node->yylloc), gen);
        }*/
        node->setRef(*var);
        node->setValue(alloc);
    }
}

void IrPass::parse(ast::Node *node, ast::IfExpression *data, Generation &gen) {
    parseNode(data->cond, gen);
    llvm::Value *condV = data->cond->getValue();

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
    parseNode(data->thenBlk, gen);
    llvm::Value *thenValue = data->thenBlk->getValue();
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
        parseNode(data->elseBlk, gen);
        elseValue = data->elseBlk->getValue();
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
        return;
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
        node->setValue(phiValue);
    }
}

void IrPass::parse(ast::Node *node, ast::Comparison *data, Generation &gen) {
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
        ast::Node *operatorNode = ast::Operator::create(*opIt, args, (*it)->yylloc);
        parseNode(operatorNode, gen);
        valueComp = operatorNode->getValue();
        if(valueComp == NULL){
            std::cout << "comparison failed" << std::endl;
            return;
        }
        if(*operatorNode->getType() != gen.boolType){
            std::cout << "comparison is not boolean" << std::endl;
            return;
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
    node->setValue(result);
}

void IrPass::parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen) {
    parseNode(data->expression, gen);
    llvm::Value* returnExpr = gen.builder.CreateRet(data->expression->getValue());
    gen.scope->setReturnReach();
    node->setValue(returnExpr);
    return;
}

void IrPass::parse(ast::Node *node, ast::WhileStmt *data, Generation &gen) {
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
    parseNode(data->cond, gen);
    llvm::Value *condV = data->cond->getValue();
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

    return;
}

} // namespace pass
} // namespace gen
} // namespace sugar
