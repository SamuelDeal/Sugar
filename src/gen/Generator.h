#ifndef SUGAR_GEN_GENERATOR_H
#define SUGAR_GEN_GENERATOR_H

#include <llvm/Module.h>
#include <llvm/IRBuilder.h>

#include "../core/Scope.h"
#include "../core/CastGraph.h"

namespace sugar {

namespace ast {
    class Node;
}

namespace gen {

class GeneratedCode;
class Generation;

typedef llvm::Value* (*NativeFunction)(std::vector<llvm::Value*>, Generation &gen);

class Generator {
public:
    Generator();
    virtual ~Generator();

    virtual GeneratedCode* generate(ast::Node *programStmts) const;

protected:
    virtual void initCore(Generation &gen) const;

    virtual llvm::Function* generatePrintfFunction(Generation &gen) const;
    virtual core::Function* generateEchoBoolFunction(llvm::Function* printfFn, Generation &gen) const;
    virtual core::Function* generateEchoIntFunction(llvm::Function* printfFn, Generation &gen) const;
    virtual core::Function* generateEchoDoubleFunction(llvm::Function* printfFn, Generation &gen) const;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATOR_H
