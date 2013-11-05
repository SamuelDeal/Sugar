#include "Block.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* Block::create(YYLTYPE yyloc) {
    return new Node(Node::eBlock, new Block(), yyloc);
}

Block::Block() {
    stmts = new std::list<Node*>();
}

Block::~Block() {
    if(stmts == NULL){
        return;
    }

    for(std::list<Node *>::iterator i = stmts->begin(); i != stmts->end(); i++){
       delete (*i);
    }
    delete stmts;
}

void Block::clear() {
    stmts = NULL;
}

} // namespace ast
} // namespace sugar
