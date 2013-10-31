#include "FunctionDeclaration.h"

#include "Node.h"
#include "Block.h"

namespace sugar {
namespace ast {

Node* FunctionDeclaration::create(Node *type, Node *id, std::list<Node *> *arguments, Node *block) {
    return new Node(Node::eFunctionDeclaration, new FunctionDeclaration(type, id, arguments, block));
}

FunctionDeclaration::FunctionDeclaration(Node *type, Node *id, std::list<Node *> *arguments, Node *block) {
    this->type = type;
    this->block = block;
    if(block != NULL){
        dynamic_cast<Block*>(block->data)->isFunction = true;
    }
    this->id = id;
    this->arguments = arguments;
}

FunctionDeclaration::~FunctionDeclaration(){
    for(std::list<Node *>::iterator i = arguments->begin(); i != arguments->end(); i++){
        delete (*i);
    }
    delete type;
    delete block;
    delete id;
    delete arguments;
}

TypeIdentifier* FunctionDeclaration::getType() const {
    return (TypeIdentifier*)type->data;
}

Identifier* FunctionDeclaration::getId() const {
    return (Identifier*)id->data;
}

Block* FunctionDeclaration::getBlock() const {
    return (Block*)block->data;
}

} // namespace ast
} // namespace sugar
