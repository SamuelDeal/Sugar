#include "Interpreter.h"

#include "../utils.h"
#include "GeneratedCode.h"
#include "../ast/Block.h"
#include "Generation.h"
#include "../ast/all.h"
#include "../core/Variable.h"

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
}

GeneratedCode* Interpreter::generate(ast::Block *block){
    Generation *generation = new Generation();
    Generation &gen = *generation;

    llvm::InitializeNativeTarget();
    gen.module = new llvm::Module("sugar", gen.context);

    GeneratedCode *result = new GeneratedCode(NULL, generation, block);
    initCore(gen);

    return result;
}


void Interpreter::run(ast::Statement *stmt, GeneratedCode *genCode) {
    static int count = 0;
    ++count;

#if DEBUG_GENERATOR
    std::cerr << "Generating code..." << std::endl;
#endif
    Generation &gen = *genCode->getGeneration();
    std::vector<llvm::Type*> argTypes;
    llvm::FunctionType *ftype = llvm::FunctionType::get(gen.voidType, llvm::makeArrayRef(argTypes), false);
    llvm::ExecutionEngine *ee = genCode->getExecutionEngine();
    if(NULL == ee){
        ee = llvm::EngineBuilder(genCode->getModule()).create();
        genCode->setExecutionEngine(ee);
    }

    ast::Block *block = genCode->getBlock();
    int stmtCount = 0;
    for(std::list<ast::Statement*>::iterator it = block->stmts.begin(); it != block->stmts.end(); it++){
        if(stmtCount >= genCode->getStatementCount()){
            parseNode(*it, gen);
        }
        ++stmtCount;
    }
    genCode->setStatementCount(stmtCount);

    llvm::Function *runFunction = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage,
        "__shell_invoke__"+ std::to_string(count), genCode->getModule());
    llvm::BasicBlock *runBlock = llvm::BasicBlock::Create(gen.context, "entry", runFunction, 0);

    gen.pushBlock(runBlock, true);
    parseNode(stmt, gen);
    gen.popBlock();

    llvm::ReturnInst::Create(gen.context, runBlock);

#if DEBUG_GENERATOR
    std::cerr << "Code is generated.\n";
#endif
#if DEBUG_IR
    // Print the bytecode in a human-readable format
    //   to see if our program compiled properly
    llvm::PassManager pm;
    pm.add(createPrintModulePass(&llvm::errs()));
    pm.run(*genCode->getModule());
#endif
#if DEBUG_GENERATOR
    std::cerr << "\n =========== Running code... =============" << std::endl;
#endif
    std::vector<llvm::GenericValue> noargs;
    llvm::GenericValue v = ee->runFunction(runFunction, noargs);
#if DEBUG_GENERATOR
    std::cerr << "\n =========== Code was run. ==============" << std::endl;
#endif
    //return v;
}

} // namespace gen
} // namespace sugar
