#ifndef SUGAR_CORE_FUNCTION_H
#define SUGAR_CORE_FUNCTION_H

#include "AbstractFunction.h"
#include "../utils/Getter.h"

namespace sugar {
namespace core {

class Function : public AbstractFunction
{
public:
    Function(const std::string &name, Type* returnType, const std::list<const Type *> &argTypes,
                       std::function<llvm::Function * ()> fnDeclGenerator,
                       std::function<bool (llvm::Function*)> fnImplGenerator);
    Function(const std::string &name, Type* returnType, const std::list<const Type *> &argTypes, ast::Node *fnDeclNode,
                       std::function<llvm::Function * ()> fnDeclGenerator);
    Function(const std::string &name, NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes);

    virtual ~Function();

    const std::string& getName() const;

protected:
    std::string _name;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_FUNCTION_H



