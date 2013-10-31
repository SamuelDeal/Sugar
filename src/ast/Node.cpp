#include "Node.h"

#include "../utils.h"

#include "../core/Type.h"
#include "NodeData.h"

namespace sugar {
namespace ast {

Node::Node(Node::Kind kind, NodeData *dataArg) {
    _kind = kind;
    _type = NULL;
    data = dataArg;
}

Node::~Node() {
    delete data;
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

void Node::reset(Node::Kind kind, NodeData *dataArg) {
    _kind = kind;
    data = dataArg;
}

} // namespace ast
} // namespace sugar
