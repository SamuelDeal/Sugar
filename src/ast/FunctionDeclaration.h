#ifndef SUGAR_AST_FUNCTIONDECLARATION_H
#define SUGAR_AST_FUNCTIONDECLARATION_H

#include "NodeData.h"

#include <cstddef>
#include <list>
#include "../parser/parser.hpp"

namespace sugar {
namespace ast {

class Node;
class TypeIdentifier;
class Identifier;
class FunctionImplementation;

class FunctionDeclaration : public NodeData
{
public:
    static Node* create(Node *type, Node *id, std::list<Node *> *arguments, Node *impl, YYLTYPE yyloc);

    FunctionDeclaration(Node *type, Node *id, std::list<Node *> *arguments, Node *impl);
    virtual ~FunctionDeclaration();

    TypeIdentifier* getType() const;
    Identifier* getId() const;
    FunctionImplementation* getImplementation() const;

    Node *type;
    Node *id;
    std::list<Node *> *arguments;
    Node *impl;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_FUNCTIONDECLARATION_H
