#ifndef SUGAR_CORE_FUNCTION_H
#define SUGAR_CORE_FUNCTION_H

#include "AbstractFunction.h"

namespace sugar {
namespace core {


class Function : public AbstractFunction
{
public:
    Function(const std::string &name, llvm::Function *fn, Type* returnType, const std::list<const Type *> &argTypes);
    Function(const std::string &name, NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes);

    virtual ~Function();

    const std::string& getName() const;

protected:
    std::string _name;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_FUNCTION_H



