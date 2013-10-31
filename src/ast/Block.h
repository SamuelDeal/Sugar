#ifndef SUGAR_AST_BLOCK_H
#define SUGAR_AST_BLOCK_H

#include "NodeData.h"
#include <list>

namespace sugar {
namespace ast {

class Node;

class Block : public NodeData
{
public:
    static Node* create();

    Block();
    virtual ~Block();
    std::list<Node *> stmts;

    bool isFunction;
    unsigned int ifCount;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_BLOCK_H
