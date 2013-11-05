#ifndef SUGAR_AST_IFEXPRESSION_H
#define SUGAR_AST_IFEXPRESSION_H

#include "NodeData.h"
#include "../parser/parser.hpp"

namespace sugar {
namespace ast {

class Node;

class IfExpression : public NodeData
{
public:
    static Node* create(Node *cond, Node *thenBlk, Node *elseBlk, YYLTYPE yyloc);
    static Node* create(Node *cond, Node *thenBlk, YYLTYPE yyloc);

    IfExpression(Node *cond, Node *thenBlk, Node *elseBlk);
    virtual ~IfExpression();

    Node *cond;
    Node *thenBlk;
    Node *elseBlk;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_IFEXPRESSION_H
