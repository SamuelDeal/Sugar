#ifndef SUGAR_GEN_GENERATEDCODE_H
#define SUGAR_GEN_GENERATEDCODE_H

namespace llvm {
    class Function;
    class Module;
    class BasicBlock;
}

namespace sugar {
namespace ast {
    class Node;
}

namespace parser {
    class ProgramNode;
}

namespace gen {

class Generation;

class GeneratedCode
{
public:
    GeneratedCode(llvm::Function *function, Generation *gen, parser::ProgramNode *block);
    ~GeneratedCode();

    llvm::Function* getFunction() const;
    llvm::Module* getModule() const;
    parser::ProgramNode * getBlock() const;
    Generation* getGeneration() const;

protected:
    llvm::Function *_function;
    Generation *_generation;
    parser::ProgramNode *_block;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATEDCODE_H
