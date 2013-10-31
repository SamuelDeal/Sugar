#ifndef SUGAR_AST_FUNCTIONCALL_H
#define SUGAR_AST_FUNCTIONCALL_H

#include <list>
#include "../utils.h"

#include "NodeData.h"
#include "../core/Function.h"

namespace sugar {
namespace ast {

class Node;
class Identifier;


class FunctionCall : public NodeData
{
public:
    static Node* create(Node *ident, bool explicitCall, std::list<Node *> *expressions);

    FunctionCall(Node *ident, bool explicitCall, std::list<Node *> *expressions);
    virtual ~FunctionCall();

    Identifier* getFunctionName() const;

    std::list<Node *> *args;
    Node *functionName;

    virtual bool isImplicitFunctionCall() const;

protected:
    bool _explicitCall;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_FUNCTIONCALL_H
