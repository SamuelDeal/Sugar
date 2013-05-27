#include "Comparison.h"

namespace sugar {
namespace ast {

Comparison::Comparison(int operatorId, std::list<Expression *> *expressions) : Expression(Node::eComparison)
{
    this->expressions = expressions;
    this->operatorId = operatorId;
}

Comparison::~Comparison(){
    for(std::list<Expression *>::iterator i = expressions->begin(); i != expressions->end(); i++){
       delete (*i);
    }
    delete expressions;
}

} // namespace ast
} // namespace sugar
