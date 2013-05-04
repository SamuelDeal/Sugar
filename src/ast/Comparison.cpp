#include "Comparison.h"

namespace sugar {
namespace ast {

Comparison::Comparison(Kind kind, std::list<Expression *> *expressions) : Expression(Node::eComparison)
{
    this->expressions = expressions;
    this->kind = kind;
}

Comparison::~Comparison(){
    for(std::list<Expression *>::iterator i = expressions->begin(); i != expressions->end(); i++){
       delete (*i);
    }
    delete expressions;
}

} // namespace ast
} // namespace sugar
