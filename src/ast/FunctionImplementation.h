#ifndef SUGAR_AST_FUNCTIONIMPLEMENTATION_H
#define SUGAR_AST_FUNCTIONIMPLEMENTATION_H

#include <list>

#include "NodeData.h"
#include "../parser/parser.hpp"

namespace sugar {
namespace ast {

class Node;
class Block;

class FunctionImplementation : public NodeData
{
public:
    static Node* create(ast::Block* block, YYLTYPE yyloc);

    FunctionImplementation();
    virtual ~FunctionImplementation();
    std::list<Node *> *stmts;

    unsigned int ifCount;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_FUNCTIONIMPLEMENTATION_H
