#ifndef SUGAR_AST_OPERATOR_H
#define SUGAR_AST_OPERATOR_H

#include <list>

#include "NodeData.h"

namespace sugar {
namespace ast {

class Node;

class Operator : public NodeData
{
public:
    static Node* create(int operatorId, std::list<Node*> *expressions, bool before = false);

    Operator(int operatorId, std::list<Node*> *expressions, bool before = false);
    virtual ~Operator();

    std::list<Node*> *args;
    int operatorId;
    bool before;

    virtual bool isImplicitFunctionCall() const;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_OPERATOR_H
