#include "Assignment.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* Assignment::create(Node *left, Node *right, YYLTYPE yyloc) {
    return new Node(Node::eAssignment, new Assignment(left, right), yyloc);
}

Assignment::Assignment(Node *leftArg, Node *rightArg) {
    left = leftArg;
    right = rightArg;
}

Assignment::~Assignment(){
    delete left;
    delete right;
}

Identifier* Assignment::getLeft() const {
    return ((Identifier*)(left->data));
}

} // namespace ast
} // namespace sugar
