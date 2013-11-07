#ifndef SUGAR_AST_ARGUMENTDECLARATION_H
#define SUGAR_AST_ARGUMENTDECLARATION_H

#include "NodeData.h"
#include "../parser/parser.hpp"
#include "../utils/utils.h"

namespace sugar {
namespace ast {

class Node;
class TypeIdentifier;
class Identifier;
class VariableDeclaration;

class ArgumentDeclaration : public NodeData {
public:
    static Node* create(Node *type, Node *id, Node *defaultValue, YYLTYPE yyloc);
    static Node* create(Node *type, Node *id, YYLTYPE yyloc);
    static Node* create(VariableDeclaration *data, YYLTYPE yyloc);

    ArgumentDeclaration(Node *type, Node *id, Node *defaultValue);
    virtual ~ArgumentDeclaration();

    TypeIdentifier* getType() const;
    Identifier* getId() const;

    Node *type;
    Node *id;
    Node *defaultValue;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_ARGUMENTDECLARATION_H
