#ifndef SUGAR_AST_BLOCK_H
#define SUGAR_AST_BLOCK_H

#include "NodeData.h"
#include <list>
#include <cstddef>
#include "../parser/parser.hpp"

namespace sugar {
namespace ast {

class Node;

class Block : public NodeData
{
public:
    static Node* create(YYLTYPE yyloc);

    Block();
    virtual ~Block();
    void clear();
    std::list<Node *> *stmts;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_BLOCK_H
