#include "Block.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* Block::create() {
    return new Node(Node::eBlock, new Block());
}

Block::Block() {
    isFunction = false;
    ifCount = 0;
}

Block::~Block() {
    for(std::list<Node *>::iterator i = stmts.begin(); i != stmts.end(); i++){
       delete (*i);
    }
}

} // namespace ast
} // namespace sugar
