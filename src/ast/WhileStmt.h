#ifndef SUGAR_AST_WHILESTMT_H
#define SUGAR_AST_WHILESTMT_H

#include "NodeData.h"
#include "../parser/parser.hpp"

namespace sugar {
namespace ast {

class Node;

class WhileStmt : public NodeData
{
public:
    static Node* create(Node *expr, Node *block, bool testFirst, YYLTYPE yyloc);
    static Node* create(Node *expr, Node *block, YYLTYPE yyloc);

    WhileStmt(Node *expr, Node *block, bool testFirst);
    virtual ~WhileStmt();

    Node *cond;
    Node *block;
    bool testFirst;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_WHILESTMT_H
