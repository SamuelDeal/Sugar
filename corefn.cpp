#include <iostream>
#include "codegen.h"
#include "node.h"
#include "functionlist.h"

using namespace std;

extern int yyparse();
extern NBlock* programBlock;


llvm::Function* createPrintfFunction(CodeGenContext& context)
{
    std::vector<llvm::Type*> printf_arg_types;
    printf_arg_types.push_back(llvm::Type::getInt8PtrTy(getGlobalContext())); //char*

    llvm::FunctionType* printf_type =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(getGlobalContext()), printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                llvm::Twine("printf"),
                context.module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

void createEchoIntFunction(CodeGenContext& context, llvm::Function* printfFn)
{
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(llvm::Type::getInt32Ty(getGlobalContext()));

    llvm::FunctionType* echo_type =
        llvm::FunctionType::get(
            llvm::Type::getVoidTy(getGlobalContext()), echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("echo_int"),
                context.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(getGlobalContext(), "entry", func, 0);
        context.pushBlock(bblock);

    const char *constValue = "%x\n";
    llvm::Constant *format_const = llvm::ConstantDataArray::getString(getGlobalContext(), constValue);
    llvm::GlobalVariable *var =
        new llvm::GlobalVariable(
            *context.module, llvm::ArrayType::get(llvm::IntegerType::get(getGlobalContext(), 8), strlen(constValue)+1),
            true, llvm::GlobalValue::PrivateLinkage, format_const, "echo_int_format");
    llvm::Constant *zero =
        llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(getGlobalContext()));

    std::vector<llvm::Constant*> indices;
    indices.push_back(zero);
    indices.push_back(zero);
    llvm::Constant *var_ref =
        llvm::ConstantExpr::getGetElementPtr(var, indices);

    std::vector<Value*> args;
    args.push_back(var_ref);

    Function::arg_iterator argsValues = func->arg_begin();
    Value* toPrint = argsValues++;
    toPrint->setName("toPrint");
    args.push_back(toPrint);

    CallInst *call = CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
    ReturnInst::Create(getGlobalContext(), bblock);
    context.popBlock();

    std::list<llvm::Type *> types;
    types.push_back(llvm::Type::getInt32Ty(getGlobalContext()));
    core::functionList.addFunction("echo", func, types);
}

void createEchoDoubleFunction(CodeGenContext& context, llvm::Function* printfFn)
{
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(llvm::Type::getDoubleTy(getGlobalContext()));

    llvm::FunctionType* echo_type =
        llvm::FunctionType::get(
            llvm::Type::getVoidTy(getGlobalContext()), echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("echo_double"),
                context.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(getGlobalContext(), "entry", func, 0);
        context.pushBlock(bblock);

    const char *constValue = "%f\n";
    llvm::Constant *format_const = llvm::ConstantDataArray::getString(getGlobalContext(), constValue);
    llvm::GlobalVariable *var =
        new llvm::GlobalVariable(
            *context.module, llvm::ArrayType::get(llvm::IntegerType::get(getGlobalContext(), 8), strlen(constValue)+1),
            true, llvm::GlobalValue::PrivateLinkage, format_const, "echo_double_format");
    llvm::Constant *zero =
        llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(getGlobalContext()));

    std::vector<llvm::Constant*> indices;
    indices.push_back(zero);
    indices.push_back(zero);
    llvm::Constant *var_ref =
        llvm::ConstantExpr::getGetElementPtr(var, indices);

    std::vector<Value*> args;
    args.push_back(var_ref);

    Function::arg_iterator argsValues = func->arg_begin();
    Value* toPrint = argsValues++;
    toPrint->setName("toPrint");
    args.push_back(toPrint);

    CallInst *call = CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
    ReturnInst::Create(getGlobalContext(), bblock);
    context.popBlock();

    std::list<llvm::Type *> types;
    types.push_back(llvm::Type::getDoubleTy(getGlobalContext()));
    core::functionList.addFunction("echo", func, types);
}

void createCoreFunctions(CodeGenContext& context){
        llvm::Function* printfFn = createPrintfFunction(context);
    createEchoIntFunction(context, printfFn);
    createEchoDoubleFunction(context, printfFn);
}

