#include "Node.h"

#include "../utils.h"

#include "../core/Type.h"

namespace sugar {
namespace ast {

Node::Node(Node::Kind kind) {
    _kind = kind;
    _type = NULL;
}

Node::~Node() {
}

Node::Kind Node::getKind() const {
    return _kind;
}

bool Node::isImplicitFunctionCall() const{
    return false;
}

const core::Type* Node::getType() const {
    return _type;
}

void Node::setType(const core::Type& type) {
    _type = &type;
}

} // namespace ast
} // namespace sugar
