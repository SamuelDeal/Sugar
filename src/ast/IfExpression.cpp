#include "IfExpression.h"

#include "../utils.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* IfExpression::create(Node *cond, Node *thenBlk, Node *elseBlk, YYLTYPE yyloc) {
    return new Node(Node::eIfExpression, new IfExpression(cond, thenBlk, elseBlk), yyloc);
}

Node* IfExpression::create(Node *cond, Node *thenBlk, YYLTYPE yyloc) {
    return new Node(Node::eIfExpression, new IfExpression(cond, thenBlk, NULL), yyloc);
}


IfExpression::IfExpression(Node *cond, Node *thenBlk, Node *elseBlk)
{
    this->cond = cond;
    this->thenBlk = thenBlk;
    this->elseBlk = elseBlk;
}

IfExpression::~IfExpression(){
    delete cond;
    delete thenBlk;
    if(elseBlk != NULL){
        delete elseBlk;
    }
}

} // namespace ast
} // namespace sugar
