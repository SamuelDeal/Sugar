#include "Constant.h"

#include "Node.h"

namespace sugar {
namespace ast {


Node* Constant::create(long long integerVal) {
    return new Node(Node::eConstant, new Constant(integerVal));
}

Node* Constant::create(double floatVal) {
    return new Node(Node::eConstant, new Constant(floatVal));
}

Node* Constant::create(bool boolVal) {
    return new Node(Node::eConstant, new Constant(boolVal));
}

Constant::Constant(long long integerVal) {
    value.integerValue = integerVal;
    _constantType = eInteger;
}

Constant::Constant(double floatVal) {
    value.floatValue = floatVal;
    _constantType = eFloat;
}

Constant::Constant(bool boolVal) {
    value.boolValue = boolVal;
    _constantType = eBoolean;
}

Constant::ConstantType Constant::getConstantType() const {
    return _constantType;
}

} // namespace ast
} // namespace sugar
