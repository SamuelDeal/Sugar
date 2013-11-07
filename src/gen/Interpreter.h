#ifndef SUGAR_GEN_INTERPRETER_H
#define SUGAR_GEN_INTERPRETER_H

#include "Generator.h"
#include "../utils/config_checked.h"

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
#if SHELL_USE_COLOR
    void run(ast::Node *stmt, ast::Node *programBlock, bool interactive, bool useColor);
#else
    void run(ast::Node *stmt, ast::Node *programBlock, bool interactive);
#endif

protected:
    struct Interpretation {
        ~Interpretation();
        llvm::ExecutionEngine *ee;
        unsigned int stmtCount;
        GeneratedCode *generatedCode;
    };

#if SHELL_USE_COLOR
    Interpretation* initProgram(ast::Node *programBlock, bool useColor);
    Interpretation* getOrCreateInterpretation(ast::Node *programBlock, bool useColor);
#else
    Interpretation* initProgram(ast::Node *programBlock);
    Interpretation* getOrCreateInterpretation(ast::Node *programBlock);
#endif


    void printResult(llvm::Value *value, ast::Node *stmt, Generation &gen) const;

    virtual void initCore(Generation &gen) const;

    virtual core::Function* generateEchoBoolFunction(Generation &gen) const;
    virtual core::Function* generateEchoIntFunction(Generation &gen) const;
    virtual core::Function* generateEchoFloatFunction(Generation &gen) const;

    core::Function* generateEchoBoolResultFunction(Generation &gen) const;
    core::Function* generateEchoIntResultFunction(Generation &gen) const;
    core::Function* generateEchoFloatResultFunction(Generation &gen) const;



    std::map<ast::Node *, Interpretation*> _interpretations;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_INTERPRETER_H
