#include "Operator.h"

namespace sugar {
namespace ast {

Operator::Operator(int operatorId, std::list<Expression*> *expressions) :
    Expression(Node::eOperator) {
    this->operatorId = operatorId;
    this->args = expressions;
}

Operator::~Operator(){
    for(std::list<Expression *>::iterator i = args->begin(); i != args->end(); i++){
       delete (*i);
    }
    delete args;
}

bool Operator::isImplicitFunctionCall() const{
    for(std::list<Expression *>::iterator i = args->begin(); i != args->end(); i++){
        if((*i)->isImplicitFunctionCall()){
            return true;
         }
    }
    return false;
}

} // namespace ast
} // namespace sugar
