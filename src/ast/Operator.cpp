#include "Operator.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* Operator::create(int operatorId, std::list<Node*> *expressions, bool before){
    return new Node(Node::eOperator, new Operator(operatorId, expressions, before));
}

Operator::Operator(int operatorId, std::list<Node*> *expressions, bool before) {
    this->operatorId = operatorId;
    this->args = expressions;
    this->before = before;
}

Operator::~Operator(){
    for(std::list<Node *>::iterator i = args->begin(); i != args->end(); i++){
       delete (*i);
    }
    delete args;
}

bool Operator::isImplicitFunctionCall() const{
    for(std::list<Node *>::iterator i = args->begin(); i != args->end(); i++){
        if((*i)->isImplicitFunctionCall()){
            return true;
         }
    }
    return false;
}

} // namespace ast
} // namespace sugar
