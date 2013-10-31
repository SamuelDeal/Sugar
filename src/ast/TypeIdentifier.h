#ifndef SUGAR_AST_TYPEIDENTIFIER_H
#define SUGAR_AST_TYPEIDENTIFIER_H

#include "NodeData.h"

#include <string>

namespace sugar {
namespace ast {

class Node;

class TypeIdentifier : public NodeData
{
public:
    static Node* create(std::string *name);
    static Node* create(Node *expression);

    TypeIdentifier(std::string *name); //type name
    TypeIdentifier(Node *expression); //type of other expr
    virtual ~TypeIdentifier();

    std::string *name;
    Node *expression;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_TYPEIDENTIFIER_H
