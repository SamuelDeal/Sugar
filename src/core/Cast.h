#ifndef SUGAR_CORE_CAST_H
#define SUGAR_CORE_CAST_H

#include "AbstractFunction.h"

namespace sugar {
namespace core {

class Cast : public AbstractFunction
{
public:
    Cast(Type* fromType, Type* toType,
                       std::function<llvm::Function * ()> fnDeclGenerator,
                       std::function<void (llvm::Function*)> fnImplGenerator);
    Cast(Type* fromType, Type* toType, ast::Node *fnDeclNode,
                       std::function<llvm::Function * ()> fnDeclGenerator);
    Cast(NativeFunction fn, Type* fromType, Type* toType);

    virtual ~Cast();

    const Type* getFrom() const;
    const Type* getTo() const;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_CAST_H
