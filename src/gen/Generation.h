#ifndef SUGAR_GEN_GENERATION_H
#define SUGAR_GEN_GENERATION_H

#include <llvm/IRBuilder.h>
#include <llvm/LLVMContext.h>

#include "../core/Scope.h"
#include "../core/CastGraph.h"
#include "../core/Type.h"

namespace llvm {
    class Module;
}

namespace sugar {
namespace gen {

class Generator;
class Interpreter;
class Compiler;

class Generation
{
public:
    Generation();
    ~Generation();
    void pushBlock(llvm::BasicBlock *block, unsigned int scopeType = core::ScopeType::Logical);
    void popBlock();
    llvm::Module* getModule() const;
    core::Scope* getCurrentFunctionScope();

protected:
    llvm::Module *module;
    llvm::IRBuilder<> builder;
    llvm::LLVMContext context;
    core::Scope rootScope;
    core::Scope *scope;
    core::CastGraph castGraph;
    core::Function *currentFunction;

    core::Type boolType;
    core::Type intType;
    core::Type floatType;
    core::Type voidType;

    llvm::ConstantInt *trueConst;
    llvm::ConstantInt *falseConst;
    llvm::Constant *intZero;

    friend class Generator;
    friend class Interpreter;
    friend class Compiler;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATION_H
