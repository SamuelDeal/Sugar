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


void Interpreter::run(ast::Statement *stmt, ast::Block *programBlock) {
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
    parseNode(stmt, gen);
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
    llvm::GenericValue v = intr.ee->runFunction(runFunction, noargs);
#if DEBUG_GENERATOR
    std::cerr << "\n =========== Code was run. ==============" << std::endl;
#endif
    //return v;
}

} // namespace gen
} // namespace sugar
