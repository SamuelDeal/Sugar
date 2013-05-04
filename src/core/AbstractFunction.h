#ifndef SUGAR_CORE_ABSTRACTFUNCTION_H
#define SUGAR_CORE_ABSTRACTFUNCTION_H

#include <string>
#include <list>

#include "../gen/AbstractGenerator.h"

namespace llvm {
    class Function;
}

namespace sugar {
namespace core {

class Type;
class CastGraph;

class AbstractFunction
{
public:
    AbstractFunction(llvm::Function *fn, Type* returnType, const std::list<const Type *> &argTypes);
    AbstractFunction(gen::NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes);

    virtual ~AbstractFunction();

    bool isNative() const;

    bool match(const std::list<const Type*> &types, const CastGraph &castGraph) const;
    gen::NativeFunction getNative() const;
    const Type* getReturnType() const;
    operator llvm::Function*() const;

protected:
    const Type *_returnType;
    std::list<const Type *> _argsTypes;
    std::list<Type *> _argsNames;
    std::list<bool> _argsHasDefaultValue;

    bool _native;
    union {
        llvm::Function *llvmFunction;
        gen::NativeFunction nativeFunction;
    } _impl;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_ABSTRACTFUNCTION_H
