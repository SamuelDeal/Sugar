#include "Comparison.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* Comparison::create(Node *expression) {
    return new Node(Node::eComparison, new Comparison(expression));
}

Comparison::Comparison(Node *expression) {
    this->expressions.push_back(expression);
}

void Comparison::add(int operatorId, Node *expression){
    this->expressions.push_back(expression);
    this->operators.push_back(operatorId);
}

Comparison::~Comparison(){
    for(std::list<Node *>::iterator i = expressions.begin(); i != expressions.end(); i++){
       delete (*i);
    }
}

} // namespace ast
} // namespace sugar
