#include "TypeIdentifier.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* TypeIdentifier::create(std::string *name) {
    return new Node(Node::eTypeIdentifier, new TypeIdentifier(name));
}

Node* TypeIdentifier::create(Node* expression) {
    return new Node(Node::eTypeIdentifier, new TypeIdentifier(expression));
}

TypeIdentifier::TypeIdentifier(std::string *name) {
    this->name = name;
    this->expression = NULL;
}

TypeIdentifier::TypeIdentifier(Node* expression) {
    this->expression = expression;
    this->name = NULL;
}

TypeIdentifier::~TypeIdentifier() {
    if(name != NULL){
        delete name;
    }
    if(expression != NULL){
        delete expression;
    }
}

} // namespace ast
} // namespace sugar
