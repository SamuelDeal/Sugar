#ifndef SUGAR_AST_COMPARISON_H
#define SUGAR_AST_COMPARISON_H

#include <list>

#include "NodeData.h"
#include "../parser/parser.hpp"

namespace sugar {
namespace ast {

class Node;

class Comparison : public NodeData
{
public:
    static Node* create(Node *expression, YYLTYPE yyloc);

    Comparison(Node *firstExpr);
    virtual ~Comparison();

    void add(int operatorId, Node *expression);
    std::list<Node *> expressions;
    std::list<int> operators;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_COMPARISON_H
