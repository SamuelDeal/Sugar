#include "ArgumentDeclaration.h"

#include "Node.h"
#include "VariableDeclaration.h"

namespace sugar {
namespace ast {

class VariableDeclaration;

Node* ArgumentDeclaration::create(Node *type, Node *id, Node *defaultValue, YYLTYPE yyloc) {
    return new Node(Node::eArgumentDeclaration, new ArgumentDeclaration(type, id, defaultValue), yyloc);
}

Node* ArgumentDeclaration::create(Node *type, Node *id, YYLTYPE yyloc) {
    return new Node(Node::eArgumentDeclaration, new ArgumentDeclaration(type, id, NULL), yyloc);
}

Node* ArgumentDeclaration::create(VariableDeclaration *data, YYLTYPE yyloc) {
    return new Node(Node::eArgumentDeclaration, new ArgumentDeclaration(data->type, data->id, data->assign), yyloc);
}

ArgumentDeclaration::ArgumentDeclaration(Node *type, Node *id, Node *defaultValue) {
    this->type = type;
    this->id = id;
    this->defaultValue = defaultValue;
}

ArgumentDeclaration::~ArgumentDeclaration(){
    delete type;
    delete id;
    if(defaultValue != NULL){
        delete defaultValue;
    }
}

TypeIdentifier* ArgumentDeclaration::getType() const {
    return (TypeIdentifier*)type->data;
}

Identifier* ArgumentDeclaration::getId() const {
    if(id == NULL){
        return NULL;
    }
    return (Identifier*)id->data;
}

} // namespace ast
} // namespace sugar
