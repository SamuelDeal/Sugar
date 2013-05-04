#ifndef SUGAR_CORE_CAST_H
#define SUGAR_CORE_CAST_H

#include "AbstractFunction.h"

namespace sugar {
namespace core {

class Cast : public AbstractFunction
{
public:
    Cast(llvm::Function *fn, Type* fromType, Type* toType);
    Cast(gen::NativeFunction fn, Type* fromType, Type* toType);

    virtual ~Cast();

    const Type* getFrom() const;
    const Type* getTo() const;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_CAST_H
