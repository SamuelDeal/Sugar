#include "Generator.h"

#include <iostream>
#include <list>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Instructions.h>
#include <llvm/InstrTypes.h>
/*
#include "../config_checked.h"
#include "../ast/all.h"
#include "../core/Type.h"

#include "../core/Variable.h"
#include "../core/Operator.h"
#include "../core/Cast.h"
#include "../parser/parser.hpp"
*/

#include "../core/Function.h"
#include "GeneratedCode.h"
#include "Generation.h"

#include "pass/FunctionLookupPass.h"
#include "pass/IrPass.h"
#include "../types/all.h"
#include "../utils/Getter.tpl.h"


namespace sugar {
namespace gen {

using namespace core;

Generator::Generator(){
}

Generator::~Generator(){
}

GeneratedCode* Generator::generate(ast::Node *programStmts) const{
#if DEBUG_GENERATOR
    std::cerr << "\n========= Generating code... =========\n";
#endif
    Generation *generation = new Generation();
    Generation &gen = *generation;
    pass::FunctionLookupPass fnPass;
    pass::IrPass irPass;

    llvm::InitializeNativeTarget();
    gen.module = new llvm::Module("sugar", gen.context);

    // Create the top level interpreter function to call as entry
    std::vector<llvm::Type*> argTypes;
    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt32Ty(gen.context), makeArrayRef(argTypes), false);
    llvm::Function *mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main", gen.module);
    llvm::BasicBlock *mainBlock = llvm::BasicBlock::Create(gen.context, "entry", mainFunction, 0);

    GeneratedCode *result = new GeneratedCode(mainFunction, generation, programStmts);
    initCore(gen);
    gen.pushBlock(mainBlock, ScopeType::Function|ScopeType::Main);
    fnPass.parseNode(programStmts, gen);
    irPass.parseNode(programStmts, gen);
    gen.builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(gen.context), 0, true));
#if DEBUG_GENERATOR
    std::cerr << "\n========= Code is generated. =========\n";
#endif
    return result;
}



void Generator::initCore(Generation &gen) const{
    gen.addInternalFunction("printf", utils::Getter<llvm::Function*>([&] {
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
    }));

    types::Bool boolType;
    boolType.generate(gen);
    gen.rootScope.addFunction(generateEchoBoolFunction(gen));

    types::Int intType;
    intType.generate(gen);
    gen.rootScope.addFunction(generateEchoIntFunction(gen));

    types::Float floatType;
    floatType.generate(gen);
    gen.rootScope.addFunction(generateEchoFloatFunction(gen));
}

Function* Generator::generateEchoBoolFunction(Generation &gen) const {
    std::list<const Type *> types;
    types.push_back(&gen.boolType);

    Function *fn = new Function("echo", &gen.voidType, types, [&] {
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
        llvm::CallInst *call = llvm::CallInst::Create(gen.getInternalFunction("printf"), makeArrayRef(args), "", bblock);
        llvm::ReturnInst::Create(gen.context, bblock);

        return func;
    });

    return fn;
}


Function* Generator::generateEchoIntFunction(Generation &gen) const {
    std::list<const Type *> types;
    types.push_back(&gen.intType);

    Function *fn = new Function("echo", &gen.voidType, types, [&] {
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

        llvm::CallInst *call = llvm::CallInst::Create(gen.getInternalFunction("printf"), makeArrayRef(args), "", bblock);
        llvm::ReturnInst::Create(gen.context, bblock);

        return func;
    });

    return fn;
}

Function* Generator::generateEchoFloatFunction(Generation &gen) const {
    std::list<const Type *> types;
    types.push_back(&gen.floatType);

    Function *fn = new Function("echo", &gen.voidType, types, [&] {
        std::vector<llvm::Type*> echo_arg_types;
        echo_arg_types.push_back(gen.floatType);

        llvm::FunctionType* echo_type = llvm::FunctionType::get(gen.voidType, echo_arg_types, false);

        llvm::Function *func = llvm::Function::Create(
                    echo_type, llvm::Function::InternalLinkage,
                    llvm::Twine("echo_double"),
                    gen.module
               );

        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", func, 0);

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

        llvm::CallInst *call = llvm::CallInst::Create(gen.getInternalFunction("printf"), makeArrayRef(args), "", bblock);
        llvm::ReturnInst::Create(gen.context, bblock);

        return func;
    });

    return fn;
}

} // namespace gen
} // namespace sugar
