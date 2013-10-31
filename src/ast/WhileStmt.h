#ifndef SUGAR_AST_WHILESTMT_H
#define SUGAR_AST_WHILESTMT_H

#include "NodeData.h"

namespace sugar {
namespace ast {

class Node;

class WhileStmt : public NodeData
{
public:
    static Node* create(Node *expr, Node *block, bool testFirst = false);

    WhileStmt(Node *expr, Node *block, bool testFirst = false);
    virtual ~WhileStmt();

    Node *cond;
    Node *block;
    bool testFirst;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_WHILESTMT_H
