#ifndef SUGAR_GEN_GENERATEDCODE_H
#define SUGAR_GEN_GENERATEDCODE_H

namespace llvm {
    class Function;
    class Module;
    class BasicBlock;
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

protected:
    llvm::Function *_function;
    Generation *_generation;
    ast::Block *_block;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATEDCODE_H
