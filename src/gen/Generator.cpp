#include "Generator.h"

#include <iostream>
#include <list>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Instructions.h>
#include <llvm/InstrTypes.h>

#include "../config.h"
#include "../ast/all.h"
#include "../core/Type.h"
#include "../core/Function.h"
#include "../core/Variable.h"
#include "../core/Operator.h"
#include "../core/Cast.h"
#include "../parser.hpp"

#include "GeneratedCode.h"
#include "Generation.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

namespace sugar {
namespace gen {

using namespace core;

Generator::Generator(){
}

Generator::~Generator(){
}

GeneratedCode* Generator::generate(ast::Block *block){
#if DEBUG_GENERATOR
    std::cerr << "\n========= Generating code... =========\n";
#endif
    Generation *generation = new Generation();
    Generation &gen = *generation;


    llvm::InitializeNativeTarget();
    gen.module = new llvm::Module("sugar", gen.context);

    // Create the top level interpreter function to call as entry
    std::vector<llvm::Type*> argTypes;
    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt32Ty(gen.context), makeArrayRef(argTypes), false);
    llvm::Function *mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main", gen.module);
    llvm::BasicBlock *mainBlock = llvm::BasicBlock::Create(gen.context, "entry", mainFunction, 0);

    GeneratedCode *result = new GeneratedCode(mainFunction, generation, block);
    initCore(gen);
    gen.pushBlock(mainBlock, ScopeType::Function|ScopeType::Main);
    parse(block, gen);
    gen.builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(gen.context), 0, true));
#if DEBUG_GENERATOR
    std::cerr << "\n========= Code is generated. =========\n";
#endif
    return result;
}

llvm::Value* Generator::generateAddIntInt(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateAdd(values[0], values[1]);
}

llvm::Value* Generator::generateAddFloatFloat(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateFAdd(values[0], values[1]);
}

llvm::Value* Generator::generateSubIntInt(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateSub(values[0], values[1]);
}

llvm::Value* Generator::generateSubFloatFloat(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateFSub(values[0], values[1]);
}

llvm::Value* Generator::generateMulIntInt(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateMul(values[0], values[1]);
}

llvm::Value* Generator::generateMulFloatFloat(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateFMul(values[0], values[1]);
}

llvm::Value* Generator::generateDivFloatFloat(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateFDiv(values[0], values[1]);
}

llvm::Value* Generator::generateEqIntInt(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateICmpEQ(values[0], values[1]);
}

llvm::Value* Generator::generateEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateFCmpUEQ(values[0], values[1]);
}

llvm::Value* Generator::generateIntToFloatCast(std::vector<llvm::Value*> values, Generation &gen){
    return gen.builder.CreateFPToSI(values[0], gen.floatType);
}

void Generator::initCore(Generation &gen){
    gen.castGraph.addImplicitCast(new Cast(&AbstractGenerator::generateIntToFloatCast, &gen.floatType, &gen.intType));

    std::list<const Type*> types;
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    Operator *op = new Operator(TPLUS, &AbstractGenerator::generateAddIntInt, &gen.intType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TPLUS, &AbstractGenerator::generateAddFloatFloat, &gen.floatType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TMINUS, &AbstractGenerator::generateSubIntInt, &gen.intType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TMINUS, &AbstractGenerator::generateSubFloatFloat, &gen.floatType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TMUL, &AbstractGenerator::generateMulIntInt, &gen.intType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TMUL, &AbstractGenerator::generateMulFloatFloat, &gen.floatType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TDIV, &AbstractGenerator::generateDivFloatFloat, &gen.floatType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TCEQ, &AbstractGenerator::generateEqIntInt, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TDIV, &AbstractGenerator::generateEqFloatFloat, &gen.boolType, types);
    gen.rootScope.addOperator(op);


    llvm::Function *printf = generatePrintfFunction(gen);

    gen.rootScope.addFunction(generateEchoBoolFunction(printf, gen));
    gen.rootScope.addFunction(generateEchoIntFunction(printf, gen));
    gen.rootScope.addFunction(generateEchoDoubleFunction(printf, gen));
}

llvm::Function* Generator::generatePrintfFunction(Generation &gen) {
    std::vector<llvm::Type*> printf_arg_types;
    printf_arg_types.push_back(llvm::Type::getInt8PtrTy(gen.context)); //char*

    llvm::FunctionType* printf_type = llvm::FunctionType::get(gen.intType, printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                llvm::Twine("printf"),
                gen.module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

Function* Generator::generateEchoBoolFunction(llvm::Function* printfFn, Generation &gen) {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.boolType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);
    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("echo_bool"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);

    const char *trueConstValue = "true\n";
    llvm::Constant *trueStrConst = llvm::ConstantDataArray::getString(gen.context, trueConstValue);
    llvm::GlobalVariable *trueVar = new llvm::GlobalVariable(
            *gen.module, llvm::ArrayType::get(llvm::IntegerType::get(gen.context, 8),
            strlen(trueConstValue)+1), true, llvm::GlobalValue::PrivateLinkage, trueStrConst, "echo_true_str");
    std::vector<llvm::Constant*> indicesTrue;
    indicesTrue.push_back(gen.intZero);
    indicesTrue.push_back(gen.intZero);
    llvm::Constant *trueRef = llvm::ConstantExpr::getGetElementPtr(trueVar, indicesTrue);

    const char *falseConstValue = "false\n";
    llvm::Constant *falseStrConst = llvm::ConstantDataArray::getString(gen.context, falseConstValue);
    llvm::GlobalVariable *falseVar = new llvm::GlobalVariable(
            *gen.module, llvm::ArrayType::get(llvm::IntegerType::get(gen.context, 8),
            strlen(falseConstValue)+1), true, llvm::GlobalValue::PrivateLinkage, falseStrConst, "echo_false_str");
    std::vector<llvm::Constant*> indicesFalse;
    indicesFalse.push_back(gen.intZero);
    indicesFalse.push_back(gen.intZero);
    llvm::Constant *falseRef = llvm::ConstantExpr::getGetElementPtr(falseVar, indicesFalse);

    llvm::Function::arg_iterator argsValues = func->arg_begin();
    llvm::Value* toPrint = argsValues++;
    toPrint->setName("toPrint");

    llvm::ICmpInst* testResult = new llvm::ICmpInst(*bblock, llvm::ICmpInst::ICMP_NE, toPrint, gen.falseConst, "");
    llvm::SelectInst* displayed = llvm::SelectInst::Create(testResult, trueRef, falseRef, "", bblock);

    std::vector<llvm::Value*> args;
    args.push_back(displayed);
    llvm::CallInst *call = llvm::CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
    llvm::ReturnInst::Create(gen.context, bblock);

    std::list<const Type *> types;
    types.push_back(&gen.boolType);
    return new Function("echo", func, &gen.voidType, types);
}

Function* Generator::generateEchoIntFunction(llvm::Function* printfFn, Generation &gen) {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.intType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("echo_int"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);
    const char *constValue = "%lld\n";

    llvm::Constant *format_const = llvm::ConstantDataArray::getString(gen.context, constValue);
    llvm::GlobalVariable *var = new llvm::GlobalVariable(
            *gen.module, llvm::ArrayType::get(llvm::IntegerType::get(gen.context, 8),
            strlen(constValue)+1), true, llvm::GlobalValue::PrivateLinkage, format_const, "echo_int_format");

    std::vector<llvm::Constant*> indices;
    indices.push_back(gen.intZero);
    indices.push_back(gen.intZero);
    llvm::Constant *var_ref = llvm::ConstantExpr::getGetElementPtr(var, indices);

    std::vector<llvm::Value*> args;
    args.push_back(var_ref);

    llvm::Function::arg_iterator argsValues = func->arg_begin();
    llvm::Value* toPrint = argsValues++;
    toPrint->setName("toPrint");
    args.push_back(toPrint);

    llvm::CallInst *call = llvm::CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
    llvm::ReturnInst::Create(gen.context, bblock);

    std::list<const Type *> types;
    types.push_back(&gen.intType);
    return new Function("echo", func, &gen.voidType, types);
}

Function* Generator::generateEchoDoubleFunction(llvm::Function* printfFn, Generation &gen) {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.floatType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("echo_double"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);
    //    context.pushBlock(bblock);

    const char *constValue = "%f\n";
    llvm::Constant *format_const = llvm::ConstantDataArray::getString(gen.context, constValue);
    llvm::GlobalVariable *var = new llvm::GlobalVariable(
            *gen.module, llvm::ArrayType::get(llvm::IntegerType::get(gen.context, 8),
            strlen(constValue)+1), true, llvm::GlobalValue::PrivateLinkage, format_const, "echo_double_format");

    std::vector<llvm::Constant*> indices;
    indices.push_back(gen.intZero);
    indices.push_back(gen.intZero);
    llvm::Constant *var_ref = llvm::ConstantExpr::getGetElementPtr(var, indices);

    std::vector<llvm::Value*> args;
    args.push_back(var_ref);

    llvm::Function::arg_iterator argsValues = func->arg_begin();
    llvm::Value* toPrint = argsValues++;
    toPrint->setName("toPrint");
    args.push_back(toPrint);

    llvm::CallInst *call = llvm::CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
    llvm::ReturnInst::Create(gen.context, bblock);
    //context.popBlock();

    std::list<const Type *> types;
    types.push_back(&gen.floatType);
    return new Function("echo", func, &gen.voidType, types);
}


llvm::Value* Generator::parse(ast::TypeIdentifier *node, Generation &gen){
    //Nothing to do
    return NULL;
}

llvm::Value* Generator::parseNode(ast::Node *node, Generation &gen){
    switch(node->getKind()){
        case ast::Node::eAssignment:            return parse((ast::Assignment *)node, gen);
        case ast::Node::eBlock:                 return parse((ast::Block *)node, gen);
        case ast::Node::eComparison:            return parse((ast::Comparison *)node, gen);
        case ast::Node::eConstant:              return parse((ast::Constant *)node, gen);
        case ast::Node::eExpressionStmt:        return parse((ast::ExpressionStmt *)node, gen);
        case ast::Node::eFunctionCall:          return parse((ast::FunctionCall *)node, gen);
        case ast::Node::eFunctionDeclaration:   return parse((ast::FunctionDeclaration *)node, gen);
        case ast::Node::eIdentifier:            return parse((ast::Identifier *)node, gen);
        case ast::Node::eIfExpression:          return parse((ast::IfExpression *)node, gen);
        case ast::Node::eReturnStmt:            return parse((ast::ReturnStmt *)node, gen);
        case ast::Node::eVariableDeclaration:   return parse((ast::VariableDeclaration *)node, gen);
        case ast::Node::eTypeIdentifier:        return parse((ast::TypeIdentifier *)node, gen);
        case ast::Node::eOperator:              return parse((ast::Operator *)node, gen);
    }
}

llvm::Value* Generator::parse(ast::Assignment *node, Generation &gen){
#if DEBUG_GENERATOR
    std::cerr << "Creating assignment for " << *node->left->name << std::endl;
#endif
    Variable *var = gen.scope->getVar(*node->left->name);
    if(var == NULL){
        std::cout << "undeclared variable " << *node->left->name << std::endl;
        return NULL;
    }
    llvm::Value *value = parseNode(node->right, gen);
    if(value == NULL){
        return NULL;
    }
    if(*node->right->getType() != var->getType()){
        std::cout << "Uncompatible types in assigment  " << node->right->getType()->getName()
                  << " to " << var->getType().getName() << std::endl;
        return NULL;
    }
    llvm::StoreInst *assign = gen.builder.CreateStore(value, *var);
    return assign;
}

llvm::Value* Generator::parse(ast::Block *node, Generation &gen){
    std::list<ast::Statement*>::const_iterator it;
    llvm::Value *lastValue = NULL;
    ast::Statement *lastStatement = NULL;
    for (it = node->stmts.begin(); it != node->stmts.end(); it++) {
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

llvm::Value* Generator::parse(ast::Constant *node, Generation &gen){
    switch(node->getConstantType()){
        case ast::Constant::eBoolean:
            node->setType(gen.boolType);
            return llvm::ConstantInt::get(gen.boolType, node->value.boolValue == false ? 0 : 1, false);
        case ast::Constant::eInteger:
            node->setType(gen.intType);
            return llvm::ConstantInt::get(gen.intType, node->value.integerValue, true);
        case ast::Constant::eFloat:
            node->setType(gen.floatType);
            return llvm::ConstantFP::get(gen.floatType, node->value.floatValue);
        default:
            std::cout << "Unknown constant type" << std::endl;
            return NULL;
    }
}

llvm::Value* Generator::parse(ast::ExpressionStmt *node, Generation &gen){
#if DEBUG_GENERATOR
    std::cerr << "Generating code for expression" << std::endl;
#endif
    llvm::Value *result = parseNode(node->expression, gen);
    node->setType(*node->expression->getType());
    return result;
}

llvm::Value* Generator::parse(ast::FunctionCall *node, Generation &gen){
#if DEBUG_GENERATOR
    std::cerr << "\n*** Method call generation " << *node->functionName->name << std::endl;
#endif
    std::vector<llvm::Value*> args;
    std::list<ast::Expression*>::const_iterator it;
    std::list<const Type *> types;
    for (it = node->args->begin(); it != node->args->end(); it++) {
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

    std::list<Function*> functions = gen.scope->getFuncs(*node->functionName->name);

    if (functions.empty()){
        std::cout << "undeclared function " << *node->functionName->name << std::endl;
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
                    call = CALL_MEMBER_FN(*this, func->getNative())(args, gen);
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

llvm::Value* Generator::parse(ast::Operator *node, Generation &gen){
#if DEBUG_GENERATOR
    std::cerr << "\n*** Operator call generation " << std::endl;
#endif
    std::vector<llvm::Value*> args;
    std::list<ast::Expression*>::const_iterator it;
    std::list<const Type *> types;
    for (it = node->args->begin(); it != node->args->end(); it++) {
        args.push_back(parseNode(*it, gen));
        types.push_back((*it)->getType());
    }

    std::list<Operator*> operators = gen.scope->getOps(node->operatorId);

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
                    call = CALL_MEMBER_FN(*this, (*it)->getNative())(args, gen);
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
        std::cout << "no operator match given arguments" << std::endl;
        return NULL;
    }
}

llvm::Value* Generator::parse(ast::Identifier *node, Generation &gen){
#if DEBUG_GENERATOR
    std::cerr << "\n*** Identifier " << *node->name << std::endl;
#endif
    Variable *var = gen.scope->getVar(*node->name);
    if(var != NULL){
        node->setType(var->getType());
        return new llvm::LoadInst(*var, "", false, *gen.scope);
    }
    else{
        std::list<Function *> functions = gen.scope->getFuncs(*node->name);
        if (functions.empty()){
            std::cout << "undeclared variable or function " << *node->name << std::endl;
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
                        call = CALL_MEMBER_FN(*this, (*it)->getNative())(args, gen);
                    }
                    else{
                        call = gen.builder.CreateCall(**it);
                    }
#if DEBUG_GENERATOR
                    std::cerr << "Creating method call " << *node->name << std::endl;
#endif
                    return call;
                }
            }
            std::cout << "undeclared variable or function " << *node->name << std::endl;
            return NULL;
        }
    }
}

llvm::Value* Generator::parse(ast::FunctionDeclaration *node, Generation &gen){
    Type *returnType = gen.scope->getType(*node->type->name);
    if(NULL == returnType){
        std::cout << "Unknown type " << *node->type->name << std::endl;
        return NULL;
    }

    std::vector<llvm::Type*> argTypes;
    std::list<ast::VariableDeclaration *>::const_iterator it;
    for (it = node->arguments->begin(); it != node->arguments->end(); it++) {
        Type *argType = gen.scope->getType(*(*it)->type->name);
        if(argType == NULL){
            std::cout << "Unknown type " << *(*it)->type->name << std::endl;
            return NULL;
        }
        argTypes.push_back(*argType);
    }
    llvm::FunctionType *ftype = llvm::FunctionType::get(*returnType, llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, node->id->name->c_str(), gen.module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", function, 0);

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;
    std::list<const Type *> types;

    gen.pushBlock(bblock, ScopeType::Function);

    for (it = node->arguments->begin(); it != node->arguments->end(); it++) {
        parse(*it, gen);
        argumentValue = argsValues++;
        argumentValue->setName((*it)->id->name->c_str());
        gen.builder.CreateStore(argumentValue, *gen.scope->getVar(*(*it)->id->name));
        types.push_back((*it)->getType());
    }

    llvm::Value *blockReturnValue = parse(node->block, gen);
    if(node->block->getType() != returnType){
        std::cout << "Function type mismatch with declaration !"  << std::endl;
        return NULL;
    }

    if(*returnType != gen.voidType){
#if DEBUG_GENERATOR
        std::cerr << "!!!! Creating Return" << std::endl;
#endif
        gen.builder.CreateRet(blockReturnValue);
    }
    else{
#if DEBUG_GENERATOR
        std::cerr << "!!!! Creating Empty Return" << std::endl;
#endif
        gen.builder.CreateRetVoid();
    }

    gen.popBlock();
    gen.scope->addFunction(new Function(*node->id->name, function, gen.scope->getType(*node->type->name), types));
#if DEBUG_GENERATOR
    std::cerr << "Creating function: " << *node->id->name << std::endl;
#endif
    return function;
}

llvm::Value* Generator::parse(ast::VariableDeclaration *node, Generation &gen){
    Type *type = gen.scope->getType(*node->type->name);
    if(type == NULL){
        std::cout << "Unknown type " << *node->type->name << std::endl;
        return NULL;
    }
    node->setType(*type);
    if(gen.scope->isGlobal()){
#if DEBUG_GENERATOR
        std::cerr << "Creating global variable declaration " << *node->type->name << " " << *node->id->name << std::endl;
#endif
        llvm::GlobalVariable *var = new llvm::GlobalVariable(*gen.module, *type, false,
            llvm::GlobalValue::InternalLinkage, 0, *node->id->name);
        var->setInitializer(llvm::Constant::getNullValue(*type));
        gen.scope->addVar(new Variable(*node->id->name, *type, var));
        if(node->assign != NULL){
#if DEBUG_GENERATOR
            std::cerr << "Assigning value to global variable declaration " << type->getName() << " " << node->id->name << std::endl;
#endif
            return parse(new ast::Assignment(node->id, node->assign), gen);
        }
        //lastGlobal = var;
        return var;
    }
    else { //local variable declaration
#if DEBUG_GENERATOR
        std::cerr << "Creating local variable declaration " << *node->type->name << " " << *node->id->name << std::endl;
#endif
        llvm::AllocaInst *alloc = new llvm::AllocaInst(*type, node->id->name->c_str(), *gen.scope);
        gen.scope->addVar(new Variable(*node->id->name, *type, alloc));
        if(node->assign != NULL){
#if DEBUG_GENERATOR
            std::cerr << "Assigning value to local variable declaration " << type->getName() << " " << *node->id->name << std::endl;
#endif
            return parse(new ast::Assignment(node->id, node->assign), gen);
        }
        return alloc;
    }
}

llvm::Value* Generator::parse(ast::IfExpression *node, Generation &gen){
    llvm::Value *condV = parseNode(node->cond, gen);

    llvm::Function *currentFunction = gen.builder.GetInsertBlock()->getParent();
    core::Scope *currentFunctionScope = gen.getCurrentFunctionScope();
    unsigned int ifCount = ++currentFunctionScope->ifCount;
    std::string ifIndex = "";
    if(ifCount > 1){
        ifIndex = std::to_string(ifCount);
    }
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(gen.context, "if"+ifIndex, currentFunction);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(gen.context, "else"+ifIndex);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(gen.context, "endif"+ifIndex);

    gen.builder.CreateCondBr(condV, thenBB, elseBB);

    gen.pushBlock(thenBB);
    gen.builder.SetInsertPoint(thenBB);
    llvm::Value *thenValue = parse(node->thenBlk, gen);

    gen.builder.CreateBr(mergeBB);
    gen.popBlock();
    thenBB = gen.builder.GetInsertBlock();


    currentFunction->getBasicBlockList().push_back(elseBB);
    gen.pushBlock(elseBB);
    gen.builder.SetInsertPoint(elseBB);
    llvm::Value *elseValue = NULL;
    if(node->elseBlk != NULL){
        elseValue = parse(node->elseBlk, gen);
    }
    gen.builder.CreateBr(mergeBB);
    gen.popBlock();
    elseBB = gen.builder.GetInsertBlock();


    currentFunction->getBasicBlockList().push_back(mergeBB);
    gen.pushBlock(mergeBB);
    gen.builder.SetInsertPoint(mergeBB);
    if(node->elseBlk == NULL || node->elseBlk->getType() != node->thenBlk->getType() ||
            *node->elseBlk->getType() == gen.voidType){
#if DEBUG_GENERATOR
        std::cerr << "skip phi value" << std::endl;
#endif
        node->setType(gen.voidType);
        //gen.builder.SetInsertPoint(mergeBB);
        return NULL;
    }
    else{
#if DEBUG_GENERATOR
        std::cerr << "generating phi value" << std::endl;
#endif
        node->setType(*node->elseBlk->getType());
        llvm::PHINode *phiValue = gen.builder.CreatePHI(*node->getType(), 2);
        phiValue->addIncoming(thenValue, thenBB);
        phiValue->addIncoming(elseValue, elseBB);
        //gen.builder.SetInsertPoint(mergeBB);
        return phiValue;
    }

}

llvm::Value* Generator::parse(ast::Comparison *node, Generation &gen){
    if(node->expressions->size() < 2){
        std::cout << "At least 2 expressions are required for comparison" << std::endl;
        return NULL;
    }
    node->setType(gen.boolType);

    std::list<ast::Expression *>::iterator it = node->expressions->begin();
    ast::Expression *left = *it;
    llvm::Value *result = NULL;
    llvm::Value *valueComp = NULL;

    for(++it; it != node->expressions->end(); it++){
        std::list<ast::Expression*> *args = new std::list<ast::Expression*>();
        args->push_back(left);
        args->push_back(*it);
        ast::Operator *operatorNode = new ast::Operator(node->operatorId, args);
        valueComp = parse(operatorNode, gen);
        if(valueComp == NULL){
            std::cout << "comparison failed" << std::endl;
            return NULL;
        }
        if(*operatorNode->getType() != gen.boolType){
            std::cout << "comparison is not boolean" << std::endl;
            return NULL;
        }
        operatorNode->args->clear();
        delete operatorNode;
        left = *it;
        if(result == NULL){
            result = valueComp;
        }
        else{
            result = gen.builder.CreateAnd(result, valueComp);
        }
    }
    return result;
}

llvm::Value* Generator::parse(ast::ReturnStmt *node, Generation &gen){
    return NULL;
}


} // namespace gen
} // namespace sugar
