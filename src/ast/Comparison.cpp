#include "Comparison.h"

namespace sugar {
namespace ast {

Comparison::Comparison(Expression *expression) : Expression(Node::eComparison)
{
    this->expressions.push_back(expression);
}

void Comparison::add(int operatorId, Expression *expression){
    this->expressions.push_back(expression);
    this->operators.push_back(operatorId);
}

Comparison::~Comparison(){
    for(std::list<Expression *>::iterator i = expressions.begin(); i != expressions.end(); i++){
       delete (*i);
    }
}

} // namespace ast
} // namespace sugar
