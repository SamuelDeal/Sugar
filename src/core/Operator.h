#ifndef SUGAR_CORE_OPERATOR_H
#define SUGAR_CORE_OPERATOR_H

#include "AbstractFunction.h"

namespace sugar {
namespace core {

class Operator : public AbstractFunction
{
public:
    Operator(int operatorId, Type* returnType, const std::list<const Type *> &argTypes,
                       std::function<llvm::Function * ()> fnDeclGenerator,
                       std::function<void (llvm::Function*)> fnImplGenerator);
    Operator(int operatorId, Type* returnType, const std::list<const Type *> &argTypes, ast::Node *fnDeclNode,
                       std::function<llvm::Function * ()> fnDeclGenerator);
    Operator(int operatorId, NativeFunction fn, Type* returnType, const std::list<const Type *> &argTypes);

    Operator(int operatorId, Type* returnType, const Type * type, bool before,
                       std::function<llvm::Function * ()> fnDeclGenerator,
                       std::function<void (llvm::Function*)> fnImplGenerator);
    Operator(int operatorId, Type* returnType, const Type * type, bool before, ast::Node *fnDeclNode,
                       std::function<llvm::Function * ()> fnDeclGenerator);
    Operator(int operatorId, NativeFunction fn, Type* returnType, const Type * type, bool before);

    virtual ~Operator();
    int getOperatorId() const;
    bool getBefore() const;

protected:
    int _operatorId;
    bool _before;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_OPERATOR_H
