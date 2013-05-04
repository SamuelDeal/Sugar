#ifndef SUGAR_AST_FUNCTIONCALL_H
#define SUGAR_AST_FUNCTIONCALL_H

#include <list>
#include "../utils.h"

#include "Expression.h"
#include "../core/Function.h"

namespace sugar {
namespace ast {

class Expression;
class Identifier;

class FunctionCall : public Expression
{
public:
    FunctionCall(Identifier *ident, bool explicitCall, std::list<Expression*> *expressions);
    virtual ~FunctionCall();

    std::list<Expression*> *args;
    Identifier *functionName;

    virtual bool isImplicitFunctionCall() const;

protected:
    bool _explicitCall;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_FUNCTIONCALL_H
