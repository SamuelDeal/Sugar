#include "Operator.h"

#include "Node.h"
#include <iostream>

namespace sugar {
namespace ast {

Node* Operator::create(int operatorId, Node *expression, bool before, YYLTYPE yyloc){
    return new Node(Node::eOperator, new Operator(operatorId, new std::list<Node *>(1, expression), before), yyloc);
}

Node* Operator::create(int operatorId, std::list<Node*> *expressions, YYLTYPE yyloc) {
    return new Node(Node::eOperator, new Operator(operatorId, expressions, false), yyloc);
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
