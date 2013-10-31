#ifndef SUGAR_AST_ASSIGNMENT_H
#define SUGAR_AST_ASSIGNMENT_H

#include "NodeData.h"
#include "Identifier.h"

namespace sugar {
namespace ast {

class Node;

class Assignment : public NodeData
{
public:
    static Node* create(Node *left, Node *right);

    Assignment(Node *left, Node *right);
    virtual ~Assignment();

    Identifier* getLeft() const;

    Node *left;
    Node *right;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_ASSIGNMENT_H
