#ifndef SUGAR_AST_RETURNSTMT_H
#define SUGAR_AST_RETURNSTMT_H

#include "NodeData.h"

namespace sugar {
namespace ast {

class Node;

class ReturnStmt : public NodeData
{
public:
    static Node* create(Node *expression);

    ReturnStmt(Node *expression);
    virtual ~ReturnStmt();

    Node* expression;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_RETURNSTMT_H
