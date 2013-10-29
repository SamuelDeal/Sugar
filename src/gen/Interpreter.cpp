#include "Interpreter.h"

#include "../config.h"
#include "../utils.h"
#include "GeneratedCode.h"
#include "../ast/Block.h"
#include "Generation.h"
#include "../ast/all.h"
#include "../core/Type.h"
#include "../core/Function.h"
#include "../core/Variable.h"
#include "../core/Operator.h"
#include "../core/Cast.h"
#include "../parser/parser.hpp"

#include <iostream>
#include <llvm/Support/TargetSelect.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>

namespace sugar {
namespace gen {

using namespace core;

Interpreter::Interpreter() {
}

Interpreter::~Interpreter() {
    std::map<ast::Block *, Interpretation*>::iterator it;
    for(it = _interpretations.begin(); it != _interpretations.end(); it++){
        delete(it->second);
        ++it;
    }
}

Interpreter::Interpretation::~Interpretation() {
    delete ee;
    delete generatedCode;
}

Interpreter::Interpretation* Interpreter::initProgram(ast::Block *block){
    Generation *generation = new Generation();
    Generation &gen = *generation;

    llvm::InitializeNativeTarget();
    gen.module = new llvm::Module("sugar", gen.context);
    initCore(gen);

    Interpretation *result = new Interpretation();
    result->stmtCount = 0;
    result->generatedCode = new GeneratedCode(NULL, generation, block);
    result->ee = llvm::EngineBuilder(result->generatedCode->getModule()).create();
    return result;
}

Interpreter::Interpretation* Interpreter::getOrCreateInterpretation(ast::Block *programBlock){
    std::map<ast::Block *, Interpretation*>::iterator it = _interpretations.find(programBlock);
    if(it == _interpretations.end()){
        it = _interpretations.insert(std::make_pair(programBlock, initProgram(programBlock))).first;
    }
    return it->second;
}

void Interpreter::initCore(Generation &gen) const {
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
    types.push_back(&gen.boolType);
    types.push_back(&gen.boolType);
    op = new Operator(TCEQ, &AbstractGenerator::generateEqBoolBool, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TCEQ, &AbstractGenerator::generateEqIntInt, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.boolType);
    types.push_back(&gen.boolType);
    op = new Operator(TCNE, &AbstractGenerator::generateNEqBoolBool, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TCNE, &AbstractGenerator::generateNEqIntInt, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TCNE, &AbstractGenerator::generateNEqFloatFloat, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TCLT, &AbstractGenerator::generateLessIntInt, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TCLT, &AbstractGenerator::generateLessFloatFloat, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TCLE, &AbstractGenerator::generateLessEqIntInt, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TCLE, &AbstractGenerator::generateLessEqFloatFloat, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TCGT, &AbstractGenerator::generateMoreIntInt, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TCGT, &AbstractGenerator::generateMoreFloatFloat, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new Operator(TCGE, &AbstractGenerator::generateMoreEqIntInt, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new Operator(TCGE, &AbstractGenerator::generateMoreEqFloatFloat, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.boolType);
    types.push_back(&gen.boolType);
    op = new Operator(TAND, &AbstractGenerator::generateAndBoolBool, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.boolType);
    types.push_back(&gen.boolType);
    op = new Operator(TOR, &AbstractGenerator::generateOrBoolBool, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    llvm::Function *printf = generatePrintfFunction(gen);

    gen.rootScope.addFunction(generateEchoBoolFunction(printf, gen));
    gen.rootScope.addFunction(generateEchoIntFunction(printf, gen));
    gen.rootScope.addFunction(generateEchoDoubleFunction(printf, gen));

    gen.rootScope.addFunction(generateEchoBoolResultFunction(printf, gen));
    gen.rootScope.addFunction(generateEchoIntResultFunction(printf, gen));
    gen.rootScope.addFunction(generateEchoDoubleResultFunction(printf, gen));
}


core::Variable* Interpreter::run(ast::Statement *stmt, ast::Block *programBlock) {
    static int count = 0;
    ++count;

#if DEBUG_GENERATOR
    std::cerr << "Generating code..." << std::endl;
#endif
    Interpretation &intr = *getOrCreateInterpretation(programBlock);
    Generation &gen = *(intr.generatedCode->getGeneration());
    std::vector<llvm::Type*> argTypes;
    llvm::FunctionType *ftype = llvm::FunctionType::get(gen.voidType, llvm::makeArrayRef(argTypes), false);

    int stmtCount = 0;
    for(std::list<ast::Statement*>::iterator it = programBlock->stmts.begin();
            it != programBlock->stmts.end(); it++){
        if(stmtCount >= intr.stmtCount){
            parseNode(*it, gen);
        }
        ++stmtCount;
    }
    intr.stmtCount = stmtCount;

    llvm::Function *runFunction = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage,
        "__shell_invoke__"+ std::to_string(count), intr.generatedCode->getModule());
    llvm::BasicBlock *runBlock = llvm::BasicBlock::Create(gen.context, "entry", runFunction, 0);

    gen.pushBlock(runBlock, ScopeType::Main|ScopeType::Function);
    llvm::Value *value = parseNode(stmt, gen);
    printResult(value, stmt, gen);
    gen.popBlock();

    //llvm::ReturnInst::Create(gen.context, runBlock);
#if DEBUG_GENERATOR
    std::cerr << "Create final empty return." << std::endl;
    std::cerr << gen.scopeHierarchy() << std::endl;
#endif
    gen.builder.CreateRetVoid();

#if DEBUG_GENERATOR
    std::cerr << "Code is generated.\n";
#endif
#if DEBUG_IR
    // Print the bytecode in a human-readable format
    //   to see if our program compiled properly
    llvm::PassManager pm;
    pm.add(createPrintModulePass(&llvm::errs()));
    pm.run(*(intr.generatedCode->getModule()));
#endif
#if DEBUG_GENERATOR
    std::cerr << "\n =========== Running code... =============" << std::endl;
#endif
    std::vector<llvm::GenericValue> noargs;
    intr.ee->runFunction(runFunction, noargs);
#if DEBUG_GENERATOR
    std::cerr << "\n =========== Code was run. ==============" << std::endl;
#endif
    if(stmt->getType() == &gen.voidType){
        return NULL;
    }
    else{
        return new core::Variable("result", *(stmt->getType()), value);
    }
}

void Interpreter::printResult(llvm::Value *value, ast::Statement *stmt, Generation &gen) const {
    if(stmt->getType() != &gen.voidType){
        std::list<Function*> functions = gen.scope->getFuncs("_ _echo_result");
        std::list<const Type *> types;
        std::vector<llvm::Value*> args;
        types.push_back(stmt->getType());
        args.push_back(value);
        for(std::list<Function *>::iterator it = functions.begin(); it != functions.end(); it++){
            Function *func = *it;
            if(func->match(types, gen.castGraph)){
                if(func->isNative()){
                    CALL_MEMBER_FN(*this, func->getNative())(args, gen);
                }
                else{
                    gen.builder.CreateCall(*func, makeArrayRef(args));
                }
            }
        }
    }
}

Function* Interpreter::generateEchoBoolFunction(llvm::Function* printfFn, Generation &gen) const {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.boolType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);
    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("echo_bool"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);
#if SHELL_USE_COLOR
    const char *trueConstValue = "\x1b[32mtrue\x1b[0m\n";
#else
    const char *trueConstValue = "true\n";
#endif
    llvm::Constant *trueStrConst = llvm::ConstantDataArray::getString(gen.context, trueConstValue);
    llvm::GlobalVariable *trueVar = new llvm::GlobalVariable(
            *gen.module, llvm::ArrayType::get(llvm::IntegerType::get(gen.context, 8),
            strlen(trueConstValue)+1), true, llvm::GlobalValue::PrivateLinkage, trueStrConst, "echo_true_str");
    std::vector<llvm::Constant*> indicesTrue;
    indicesTrue.push_back(gen.intZero);
    indicesTrue.push_back(gen.intZero);
    llvm::Constant *trueRef = llvm::ConstantExpr::getGetElementPtr(trueVar, indicesTrue);

#if SHELL_USE_COLOR
    const char *falseConstValue = "\x1b[32mfalse\x1b[0m\n";
#else
    const char *falseConstValue = "false\n";
#endif
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

Function* Interpreter::generateEchoIntFunction(llvm::Function* printfFn, Generation &gen) const {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.intType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("echo_int"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);

#if SHELL_USE_COLOR
    const char *constValue = "\x1b[32m%lld\n\x1b[0m\n";
#else
    const char *constValue = "%lld\n";
#endif
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

Function* Interpreter::generateEchoDoubleFunction(llvm::Function* printfFn, Generation &gen) const {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.floatType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("echo_double"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);

#if SHELL_USE_COLOR
    const char *constValue = "\x1b[32m%f\n\x1b[0m\n";
#else
    const char *constValue = "%f\n";
#endif
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

Function* Interpreter::generateEchoBoolResultFunction(llvm::Function* printfFn, Generation &gen) const {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.boolType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);
    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("_ _echo_bool_result"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);
#if SHELL_USE_COLOR
    const char *trueConstValue = "=> \x1b[33mtrue\x1b[0m\n";
#else
    const char *trueConstValue = "=> true\n";
#endif
    llvm::Constant *trueStrConst = llvm::ConstantDataArray::getString(gen.context, trueConstValue);
    llvm::GlobalVariable *trueVar = new llvm::GlobalVariable(
            *gen.module, llvm::ArrayType::get(llvm::IntegerType::get(gen.context, 8),
            strlen(trueConstValue)+1), true, llvm::GlobalValue::PrivateLinkage, trueStrConst, "echo_true_str");
    std::vector<llvm::Constant*> indicesTrue;
    indicesTrue.push_back(gen.intZero);
    indicesTrue.push_back(gen.intZero);
    llvm::Constant *trueRef = llvm::ConstantExpr::getGetElementPtr(trueVar, indicesTrue);

#if SHELL_USE_COLOR
    const char *falseConstValue = "=> \x1b[33mfalse\x1b[0m\n";
#else
    const char *falseConstValue = "=> false\n";
#endif
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
    return new Function("_ _echo_result", func, &gen.voidType, types);
}

Function* Interpreter::generateEchoIntResultFunction(llvm::Function* printfFn, Generation &gen) const {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.intType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("_ _echo_int"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);

#if SHELL_USE_COLOR
    const char *constValue = "=> \x1b[33m%lld\n\x1b[0m\n";
#else
    const char *constValue = "=> %lld\n";
#endif
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
    return new Function("_ _echo_result", func, &gen.voidType, types);
}

Function* Interpreter::generateEchoDoubleResultFunction(llvm::Function* printfFn, Generation &gen) const {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(gen.floatType);

    llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("_ _echo_double_result"),
                gen.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);

#if SHELL_USE_COLOR
    const char *constValue = "=> \x1b[33m%f\n\x1b[0m\n";
#else
    const char *constValue = "=> %f\n";
#endif
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
    return new Function("_ _echo_result", func, &gen.voidType, types);
}

} // namespace gen
} // namespace sugar
