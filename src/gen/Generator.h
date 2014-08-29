#ifndef SUGAR_GEN_GENERATOR_H
#define SUGAR_GEN_GENERATOR_H

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "../core/Scope.h"
#include "../core/CastGraph.h"
#include "../utils/Getter.h"

namespace sugar {

namespace ast {
    class Node;
}

namespace parser {
    class ProgramNode;
}

namespace gen {

class GeneratedCode;
class Generation;

typedef llvm::Value* (*NativeFunction)(std::vector<llvm::Value*>, Generation &gen);

class Generator {
public:
    Generator();
    virtual ~Generator();

    virtual GeneratedCode* generate(parser::ProgramNode *programStmts) const;

protected:
    virtual void initCore(Generation &gen) const;

    virtual core::Function* generateEchoBoolFunction(Generation &gen) const;
    virtual core::Function* generateEchoIntFunction(Generation &gen) const;
    virtual core::Function* generateEchoFloatFunction(Generation &gen) const;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATOR_H
