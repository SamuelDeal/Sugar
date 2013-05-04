#ifndef SUGAR_GEN_GENERATEDCODE_H
#define SUGAR_GEN_GENERATEDCODE_H

namespace llvm {
    class Function;
    class Module;
    class BasicBlock;
    class ExecutionEngine;
}

namespace sugar {
namespace ast {
    class Block;
}

namespace gen {

class Generation;

class GeneratedCode
{
public:
    GeneratedCode(llvm::Function *function, Generation *gen, ast::Block *block);
    ~GeneratedCode();

    llvm::Function* getFunction() const;
    llvm::Module* getModule() const;
    ast::Block* getBlock() const;
    Generation* getGeneration() const;
    int getStatementCount() const;
    void setStatementCount(int count);
    llvm::ExecutionEngine* getExecutionEngine() const;
    void setExecutionEngine(llvm::ExecutionEngine *ee);

protected:
    llvm::Function *_function;
    Generation *_generation;
    ast::Block *_block;
    int _statementCount;
    llvm::ExecutionEngine *_ee;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATEDCODE_H
