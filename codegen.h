#ifndef CODEGEN_H
#define CODEGEN_H


#include <stack>
#include <typeinfo>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/Instructions.h>
#include <llvm/CallingConv.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
//#include <llvm/ModuleProvider.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

class NBlock;
class NStatement;

class CodeGenBlock {
public:
    BasicBlock *block;
    std::map<std::string, Value*> locals;
};

class CodeGenContext {
protected:
    std::stack<CodeGenBlock *> blocks;
    Function *mainFunction;
    std::map<std::string, Value*> _globals;

public:
    Module *module;
    CodeGenContext() { module = new Module("main", getGlobalContext());}

    void generateCode(NBlock& root);
    GenericValue runCode();
    std::map<std::string, Value*>& locals() { return blocks.top()->locals; }
    std::map<std::string, Value*>& globals() { return _globals; }
    virtual BasicBlock *currentBlock() { return blocks.top()->block; }
    //BasicBlock *currentBlock() { return (currentBlockOverride == NULL ? (blocks.top()->block) : currentBlockOverride); }
    void pushBlock(BasicBlock *block) { blocks.push(new CodeGenBlock()); blocks.top()->block = block; }
    void popBlock() { CodeGenBlock *top = blocks.top(); blocks.pop(); delete top; }
    virtual bool isGlobalContext() { return false;  }
};

class InteractiveCodeGenContext : public CodeGenContext {
    BasicBlock *currentBlockOverride;
    ExecutionEngine *ee;
    int stmtIndex;
    NBlock& root;

public:
    InteractiveCodeGenContext(NBlock &block): root(block) { ee = NULL; currentBlockOverride = NULL; stmtIndex = 0; }

    void runStatement(NStatement *stmt);
    std::map<std::string, Value*>& locals() { return blocks.top()->locals; }
    //BasicBlock *currentBlock() { return blocks.top()->block; }
    virtual BasicBlock *currentBlock() { return (currentBlockOverride == NULL ? (blocks.empty() ? NULL : blocks.top()->block) : currentBlockOverride); }
    virtual bool isGlobalContext() { return blocks.empty();  }
};


#endif // CODEGEN_H
