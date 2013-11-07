#ifndef SUGAR_GEN_GENERATION_H
#define SUGAR_GEN_GENERATION_H

#include <llvm/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <list>

#include "../core/Scope.h"
#include "../core/CastGraph.h"
#include "../core/Type.h"
#include "../utils/config_checked.h"
#include "../utils/Getter.h"

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
    std::string scopeHierarchy();

public:
    // Used by passes

    llvm::Module *module;
    llvm::IRBuilder<> builder;
    llvm::LLVMContext context;
    core::Scope rootScope;
    core::Scope *scope;
    core::CastGraph castGraph;
    core::Function *currentFunction;

#if SHELL_USE_COLOR
    bool useColor;
#endif

    core::Type boolType;
    core::Type intType;
    core::Type floatType;
    core::Type voidType;

    llvm::ConstantInt *trueConst;
    llvm::ConstantInt *falseConst;
    llvm::Constant *intZero;

    llvm::Function* getInternalFunction(const std::string &name);
    void addInternalFunction(const std::string &name, utils::Getter<llvm::Function*>);

protected:
    std::map<const std::string, utils::Getter<llvm::Function*> > _internalFunctions;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATION_H
