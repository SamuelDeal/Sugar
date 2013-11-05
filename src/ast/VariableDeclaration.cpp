#include "VariableDeclaration.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* VariableDeclaration::create(Node *type, Node *id, Node *assign, YYLTYPE yyloc) {
    return new Node(Node::eVariableDeclaration, new VariableDeclaration(type, id, assign), yyloc);
}

Node* VariableDeclaration::create(Node *type, Node *id, YYLTYPE yyloc) {
    return new Node(Node::eVariableDeclaration, new VariableDeclaration(type, id, NULL), yyloc);
}

VariableDeclaration::VariableDeclaration(Node *type, Node *id, Node *assign) {
    this->type = type;
    this->id = id;
    this->assign = assign;
}

VariableDeclaration::~VariableDeclaration(){
    delete type;
    if(id != NULL){
        delete id;
    }
    if(assign != NULL){
        delete assign;
    }
}

TypeIdentifier* VariableDeclaration::getType() const {
    return (TypeIdentifier*)type->data;
}

Identifier* VariableDeclaration::getId() const {
    if(id == NULL){
        return NULL;
    }
    return (Identifier*)id->data;
}

} // namespace ast
} // namespace sugar
