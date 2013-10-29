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

    //virtual GeneratedCode* generate(ast::Block *block);

    void run(ast::Statement *stmt, ast::Block *programBlock);

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
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_INTERPRETER_H
