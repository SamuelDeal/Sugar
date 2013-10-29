#ifndef SUGAR_GEN_INTERPRETER_H
#define SUGAR_GEN_INTERPRETER_H

#include "Generator.h"

namespace llvm {
    class ExecutionEngine;
}

namespace sugar {
namespace ast {
    class Statement;
}

namespace gen {

class Interpreter : public Generator
{
public:
    Interpreter();
    virtual ~Interpreter();

    core::Variable* run(ast::Statement *stmt, ast::Block *programBlock);

protected:
    struct Interpretation {
        ~Interpretation();
        llvm::ExecutionEngine *ee;
        unsigned int stmtCount;
        GeneratedCode *generatedCode;
    };

    std::map<ast::Block *, Interpretation*> _interpretations;

    Interpretation* initProgram(ast::Block *programBlock);
    Interpretation* getOrCreateInterpretation(ast::Block *programBlock);

    void printResult(llvm::Value *value, ast::Statement *stmt, Generation &gen) const;

    virtual void initCore(Generation &gen) const;

    virtual core::Function* generateEchoBoolFunction(llvm::Function* printfFn, Generation &gen) const;
    virtual core::Function* generateEchoIntFunction(llvm::Function* printfFn, Generation &gen) const;
    virtual core::Function* generateEchoDoubleFunction(llvm::Function* printfFn, Generation &gen) const;

    core::Function* generateEchoBoolResultFunction(llvm::Function* printfFn, Generation &gen) const;
    core::Function* generateEchoIntResultFunction(llvm::Function* printfFn, Generation &gen) const;
    core::Function* generateEchoDoubleResultFunction(llvm::Function* printfFn, Generation &gen) const;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_INTERPRETER_H
