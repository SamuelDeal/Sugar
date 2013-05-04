#include "IfExpression.h"

#include "../utils.h"

#include "Block.h"

namespace sugar {
namespace ast {

IfExpression::IfExpression(Expression *cond, Block *thenBlk, Block *elseBlk) : Expression(Node::eIfExpression)
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
