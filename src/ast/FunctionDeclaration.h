#ifndef SUGAR_AST_FUNCTIONDECLARATION_H
#define SUGAR_AST_FUNCTIONDECLARATION_H

#include "NodeData.h"

#include <cstddef>
#include <list>

namespace sugar {
namespace ast {

class Node;
class TypeIdentifier;
class Identifier;
class Block;

class FunctionDeclaration : public NodeData
{
public:
    static Node* create(Node *type, Node *id, std::list<Node *> *arguments, Node *block);

    FunctionDeclaration(Node *type, Node *id, std::list<Node *> *arguments, Node *block);
    virtual ~FunctionDeclaration();

    TypeIdentifier* getType() const;
    Identifier* getId() const;
    Block* getBlock() const;

    Node *type;
    Node *id;
    std::list<Node *> *arguments;
    Node *block;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_FUNCTIONDECLARATION_H
