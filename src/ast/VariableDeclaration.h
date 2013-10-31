#ifndef SUGAR_AST_VARIABLEDECLARATION_H
#define SUGAR_AST_VARIABLEDECLARATION_H

#include "NodeData.h"

#include "../utils.h"

namespace sugar {
namespace ast {

class Node;
class TypeIdentifier;
class Identifier;

class VariableDeclaration : public NodeData
{
public:
    static Node* create(Node *type, Node *id, Node *assign = NULL);

    VariableDeclaration(Node *type, Node *id, Node *assign = NULL);
    virtual ~VariableDeclaration();

    TypeIdentifier* getType() const;
    Identifier* getId() const;

    Node *type;
    Node *id;
    Node *assign;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_VARIABLEDECLARATION_H
