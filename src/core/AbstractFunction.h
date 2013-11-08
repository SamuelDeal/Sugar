#ifndef SUGAR_CORE_ABSTRACTFUNCTION_H
#define SUGAR_CORE_ABSTRACTFUNCTION_H

#include <string>
#include <list>
#include <vector>

#include <llvm/Function.h>
#include <functional>

namespace llvm {
    class Function;
    class Value;
}

namespace sugar {

namespace ast {
    class Node;
}

namespace gen {
    class Generation;
}

namespace core {

class Type;
class CastGraph;

typedef llvm::Value* (*NativeFunction)(std::vector<ast::Node*>, gen::Generation &gen);

class AbstractFunction
{
public:
    AbstractFunction(std::function<llvm::Function * ()> fnDeclGenerator,
                     std::function<bool (llvm::Function*)> fnImplGenerator,
                     Type* returnType, const std::list<const Type *> &argTypes);
    AbstractFunction(std::function<llvm::Function * ()> fnDeclGenerator, ast::Node *funcDecl,
                     Type* returnType, const std::list<const Type *> &argTypes);
    AbstractFunction(NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes);

    virtual ~AbstractFunction();

    bool isNative() const;

    bool match(const std::list<const Type*> &types, const CastGraph &castGraph) const;
    NativeFunction getNative() const;
    const Type* getReturnType() const;
    operator llvm::Function*();
    void setImplementationGenerator(std::function<bool (llvm::Function*)> generator);
    ast::Node *getDeclaration() const;

protected:
    bool isUsed() const;

    const Type *_returnType;
    std::list<const Type *> _argsTypes;
    std::list<Type *> _argsNames;
    std::list<bool> _argsHasDefaultValue;


    bool _native;
    struct {
        NativeFunction nativeFunction;
        struct {
            llvm::Function *llvmFunction;
            std::function<llvm::Function* ()> fnDeclGenerator;
            std::function<bool (llvm::Function*)> fnImplGenerator;
            ast::Node *funcDeclNode;
            bool implError;
        } irFunction;
    } _impl;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_ABSTRACTFUNCTION_H
