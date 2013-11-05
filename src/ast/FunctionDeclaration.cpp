#include "FunctionDeclaration.h"

#include "Node.h"
#include "FunctionImplementation.h"

namespace sugar {
namespace ast {

Node* FunctionDeclaration::create(Node *type, Node *id, std::list<Node *> *arguments, Node *impl, YYLTYPE yyloc) {
    return new Node(Node::eFunctionDeclaration, new FunctionDeclaration(type, id, arguments, impl), yyloc);
}

FunctionDeclaration::FunctionDeclaration(Node *type, Node *id, std::list<Node *> *arguments, Node *impl) {
    this->type = type;
    this->impl = impl;
    this->id = id;
    this->arguments = arguments;
}

FunctionDeclaration::~FunctionDeclaration(){
    for(std::list<Node *>::iterator i = arguments->begin(); i != arguments->end(); i++){
        delete (*i);
    }
    delete type;
    delete impl;
    delete id;
    delete arguments;
}

TypeIdentifier* FunctionDeclaration::getType() const {
    return (TypeIdentifier*)type->data;
}

Identifier* FunctionDeclaration::getId() const {
    return (Identifier*)id->data;
}

FunctionImplementation* FunctionDeclaration::getImplementation() const {
    return (FunctionImplementation*)(impl->data);
}

} // namespace ast
} // namespace sugar
