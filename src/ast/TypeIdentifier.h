#ifndef SUGAR_AST_TYPEIDENTIFIER_H
#define SUGAR_AST_TYPEIDENTIFIER_H

#include "NodeData.h"
#include "../parser/parser.hpp"

#include <string>

namespace sugar {
namespace ast {

class Node;

class TypeIdentifier : public NodeData
{
public:
    static Node* create(std::string *name, YYLTYPE yyloc);
    static Node* create(Node *expression, YYLTYPE yyloc);

    TypeIdentifier(std::string *name); //type name
    TypeIdentifier(Node *expression); //type of other expr
    virtual ~TypeIdentifier();

    std::string *name;
    Node *expression;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_TYPEIDENTIFIER_H
