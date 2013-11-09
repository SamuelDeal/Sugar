#include "IrPass.h"

#include <iostream>

#include "../../utils/config_checked.h"
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

bool IrPass::parse(ast::Node *node, ast::ArgumentDeclaration *data, Generation &gen) {
    Type *type = gen.scope->getType(*data->getType()->name);
    if(type == NULL){
        gen.addError("unknown type " + *data->getType()->name, &data->type->yylloc);
        return false;
    }
    node->setType(*type);
    Scope *scope = gen.scope->getCurrentVarOwnerScope();
#if DEBUG_GENERATOR
    std::cerr << "Creating argument declaration " << *data->getType()->name << " " << *data->getId()->name << std::endl;
#endif
    llvm::AllocaInst *alloc = new llvm::AllocaInst(*type, data->getId()->name->c_str(), *scope);
    Variable *var = new Variable(*data->getId()->name, *type, alloc);
    scope->addVar(var);
    node->setRef(*var);
    node->setValue(alloc);
    return true;
}

bool IrPass::parse(ast::Node *node, ast::Assignment *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::cerr << "Creating assignment" << std::endl;
#endif
    parseNode(data->left, gen);
    llvm::Value* leftRef = data->left->getRef();
    if(leftRef == NULL){
        gen.addError("left part is not assignable", &data->left->yylloc);
        return false;
    }
    if(!parseNode(data->right, gen)){
        return false;
    }
    if(*data->right->getType() != *data->left->getType()){
        gen.addError("uncompatible types in assigment", &data->left->yylloc,
                     data->right->getType()->getName() + " to " + data->left->getType()->getName());
        return false;
    }
    gen.builder.CreateStore(data->right->getValue(), leftRef);
    node->setType(gen.voidType);
    return true;
}

bool IrPass::parse(ast::Node *node, ast::Block *data, Generation &gen) {
    bool succeed = true;
    std::list<ast::Node*>::const_iterator it;
    llvm::Value *lastValue = NULL;
    ast::Node *lastStatement = NULL;
    for (it = data->stmts->begin(); it != data->stmts->end() && !gen.scope->isReturnReach(); it++) {
#if DEBUG_GENERATOR
        std::cerr << "Generating code for statement" << std::endl;
#endif
        lastStatement = *it;
        if(parseNode(lastStatement, gen)){
            lastValue = (*it)->getValue();
        }
        else{
            succeed = false;
            if(gen.maxErrorCountReached()){
                return false;
            }
        }
    }
#if DEBUG_GENERATOR
    std::cerr << "Creating block" << std::endl;
#endif
    if(!succeed){
        return false;
    }

    if(lastValue == NULL || lastStatement == NULL || lastStatement->getType() == &gen.voidType){
        node->setType(gen.voidType);
    }
    else{
        node->setType(*lastStatement->getType());
        node->setValue(lastValue);
    }
    return true;
}

bool IrPass::parse(ast::Node *node, ast::Comparison *data, Generation &gen) {
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
        if(!parseNode(operatorNode, gen)){
            return false;
        }
        valueComp = operatorNode->getValue();
        if(valueComp == NULL){
            gen.addError("invalid operation", &operatorNode->yylloc);
            return false;
        }
        if(*operatorNode->getType() != gen.boolType){
            gen.addError("comparison is not boolean", &operatorNode->yylloc);
            return false;
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
    return true;
}

bool IrPass::parse(ast::Node *node, ast::Constant *data, Generation &gen) {
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
            gen.addError("unknown constant type", &node->yylloc);
            return false;
    }
    return true;
}

bool IrPass::parse(ast::Node *node, ast::FunctionCall *data, Generation &gen) {
    std::string *functionName = data->getFunctionName()->name;
#if DEBUG_GENERATOR
    std::cerr << "\n*** Method call generation " << *functionName << std::endl;
#endif

    std::vector<llvm::Value*> args;
    std::vector<ast::Node*> nodeArgs;
    std::list<ast::Node*>::const_iterator it;
    std::list<const Type *> types;
    for (it = data->args->begin(); it != data->args->end(); it++) {
        if(!parseNode(*it, gen)){
            gen.addError("bad argument, function call abort", &(*it)->yylloc);
            return false;
        }
        nodeArgs.push_back(*it);
        args.push_back((*it)->getValue());
        const Type* argType = (*it)->getType();
        if(argType == NULL){
            gen.addError("bad argument, no type detected", &(*it)->yylloc);
            return false;
        }
        else {
            types.push_back(argType);
        }
    }

    std::list<Function*> functions = gen.scope->getFuncs(*functionName);

    if (functions.empty()){
        gen.addError("undeclared function " + *functionName, &data->functionName->yylloc);
        return false;
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
                    llvm::Function *irFunction = *func;
                    if(irFunction == NULL){ //function parsing failed
                        return false;
                    }
                    call = gen.builder.CreateCall(irFunction, makeArrayRef(args));
                }
#if DEBUG_GENERATOR
                std::cerr << "Creating method call " << std::endl;
#endif
                node->setValue(call);
                return true;
            }
        }
        gen.addError("no function match given arguments", &data->functionName->yylloc);
        return false;
    }
}

bool IrPass::parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen) {
    core::Function* fn = gen.scope->getByDeclarationNode(node);
    if(fn == NULL){
        return false;
    }
    fn->setImplementationGenerator([=, &gen] (llvm::Function *function) -> bool {
        Type *returnType = gen.scope->getType(*data->getType()->name);
        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", function, 0);

        llvm::Function::arg_iterator argsValues = function->arg_begin();
        llvm::Value* argumentValue;
        gen.pushBlock(bblock, ScopeType::Function);
        std::list<ast::Node*>::const_iterator it;
        bool succeed = true;
        for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
            if(!parseNode(*it, gen)){
                succeed = false;
                if(gen.maxErrorCountReached()){
                    gen.popBlock();
                    return false;
                }
            }
            argumentValue = argsValues++;
            std::string *argumentName = ((ast::ArgumentDeclaration*)((*it)->data))->getId()->name;
            argumentValue->setName(argumentName->c_str());
            gen.builder.CreateStore(argumentValue, *gen.scope->getVar(*argumentName));
        }
        if(!succeed){
            gen.popBlock();
            return false;
        }

        if(!parseNode(data->impl, gen)){
            gen.popBlock();
            return false;
        }
        llvm::Value *blockReturnValue = data->impl->getValue();
        const Type *implType = data->impl->getType();
        if(implType != returnType){

            gen.addError("function type mismatch with declaration", &data->id->yylloc,
                         "declared type: "+returnType->getName()+", effective return: "+implType->getName());
            gen.popBlock();
            return false;
        }

        if(*returnType != gen.voidType){
            gen.builder.CreateRet(blockReturnValue);
        }
        else{
            gen.builder.CreateRetVoid();
        }

        gen.popBlock();
        return true;
    });
    return true;
}

bool IrPass::parse(ast::Node *node, ast::FunctionImplementation *data, Generation &gen) {
    std::list<ast::Node*>::const_iterator it;
    llvm::Value *lastValue = NULL;
    ast::Node *lastStatement = NULL;
    bool succeed = true;
    for (it = data->stmts->begin(); it != data->stmts->end() && !gen.scope->isReturnReach(); it++) {
#if DEBUG_GENERATOR
        std::cerr << "Generating code for statement" << std::endl;
#endif
        lastStatement = *it;
        if(parseNode(lastStatement, gen)){
            lastValue = (*it)->getValue();
        }
        else{
            succeed = false;
            if(gen.maxErrorCountReached()){
                return false;
            }
        }
    }

    if(!succeed){
        return false;
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
    return true;
}

bool IrPass::parse(ast::Node *node, ast::Identifier *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::cerr << "\n*** Identifier " << *data->name << std::endl;
#endif
    Variable *var = gen.scope->getVar(*data->name);
    if(var != NULL){
        node->setType(var->getType());
        core::Scope *scope = gen.scope;
        node->setValue(utils::Getter<llvm::Value*>([=] {
            return new llvm::LoadInst(*var, "", false, *scope);
        }));
        node->setRef(*var);
    }
    else{
        std::list<Function *> functions = gen.scope->getFuncs(*data->name);
        if (functions.empty()){
            gen.addError("undeclared variable or function " + *data->name, &node->yylloc);
            return false;
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
                        llvm::Function *irFunction = **it;
                        if(irFunction == NULL){ //function parsing failed
                            return false;
                        }
                        call = gen.builder.CreateCall(irFunction);
                    }
#if DEBUG_GENERATOR
                    std::cerr << "Creating method call " << *data->name << std::endl;
#endif
                    node->setValue(call);
                    return true;
                }
            }
            gen.addError("undeclared variable or function " + *data->name, &node->yylloc);
            return false;
        }
    }
    return true;
}



bool IrPass::parse(ast::Node *node, ast::IfExpression *data, Generation &gen) {
    if(!parseNode(data->cond, gen)){
        return false;
    }
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
    if(!parseNode(data->thenBlk, gen)){
        gen.popBlock();
        return false;
    }
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
        if(!parseNode(data->elseBlk, gen)){
            gen.popBlock();
            return false;
        }
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
        return true;
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
    return true;
}

bool IrPass::parse(ast::Node *node, ast::Operator *data, Generation &gen) {
#if DEBUG_GENERATOR
    std::string debugTypes;
#endif
    std::vector<llvm::Value*> args;
    std::vector<ast::Node*> nodeArgs;
    std::list<ast::Node*>::const_iterator it;
    std::list<const Type *> types;
    for (it = data->args->begin(); it != data->args->end(); it++) {
        if(!parseNode(*it, gen)){
            return false;
        }
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
        gen.addError("unknown operator", &node->yylloc);
        return false;
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
                    llvm::Function *irFunction = **it;
                    if(irFunction == NULL){ //function parsing failed
                        return false;
                    }
                    call = gen.builder.CreateCall(irFunction, makeArrayRef(args));
                }
#if DEBUG_GENERATOR
                std::cerr << "Creating operator call " << std::endl;
#endif
                node->setValue(call);
                return true;
            }
        }
        gen.addError("no operator match given arguments", &node->yylloc);
        return false;
    }
}

bool IrPass::parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen) {
    if(!parseNode(data->expression, gen)){
        return false;
    }
    llvm::Value* returnExpr = gen.builder.CreateRet(data->expression->getValue());
    gen.scope->setReturnReach();
    node->setType(*data->expression->getType());
    node->setValue(returnExpr);
    return true;
}

bool IrPass::parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen) {
    //Nothing to do
    return true;
}

bool IrPass::parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen) {
    Type *type = gen.scope->getType(*data->getType()->name);
    if(type == NULL){
        gen.addError("unknown type " + *data->getType()->name, &data->type->yylloc);
        return false;
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
        globalVar->setInitializer(llvm::Constant::getNullValue(*type));
        scope->addVar(var);
        node->setRef(*var);
        node->setValue(globalVar);
    }
    else { //local variable declaration
#if DEBUG_GENERATOR
        std::cerr << "Creating local variable declaration " << *data->getType()->name << " " << *data->getId()->name << std::endl;
#endif
        llvm::AllocaInst *alloc = new llvm::AllocaInst(*type, data->getId()->name->c_str(), *scope);
        Variable *var = new Variable(*data->getId()->name, *type, alloc);
        scope->addVar(var);
        node->setRef(*var);
        node->setValue(alloc);
    }
    return true;
}

bool IrPass::parse(ast::Node *node, ast::WhileStmt *data, Generation &gen) {
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
    if(!parseNode(data->cond, gen)){
        gen.popBlock();
        return false;
    }
    llvm::Value *condV = data->cond->getValue();
    gen.builder.CreateCondBr(condV, loopBB, mergeBB);
    gen.popBlock();

    currentFunction->getBasicBlockList().push_back(loopBB);
    gen.pushBlock(loopBB);
    if(!parseNode(data->block, gen)){
        gen.popBlock();
        return false;
    }
    if(!gen.scope->isReturnReach()){
        gen.builder.CreateBr(condBB);
    }
    loopBB = gen.builder.GetInsertBlock();
    gen.popBlock();

    currentFunction->getBasicBlockList().push_back(mergeBB);
    gen.pushBlock(mergeBB);
    gen.scope->getParent()->replaceBlock(mergeBB);
    gen.popBlock();

    return true;
}

} // namespace pass
} // namespace gen
} // namespace sugar
