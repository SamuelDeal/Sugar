#include "Constant.h"

#include "Node.h"

namespace sugar {
namespace ast {


Node* Constant::create(long long integerVal, YYLTYPE yyloc) {
    return new Node(Node::eConstant, new Constant(integerVal), yyloc);
}

Node* Constant::create(double floatVal, YYLTYPE yyloc) {
    return new Node(Node::eConstant, new Constant(floatVal), yyloc);
}

Node* Constant::create(bool boolVal, YYLTYPE yyloc) {
    return new Node(Node::eConstant, new Constant(boolVal), yyloc);
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
