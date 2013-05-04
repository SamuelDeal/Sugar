#include "Constant.h"

namespace sugar {
namespace ast {

Constant::Constant(long long integerVal) : Expression(Node::eConstant) {
    value.integerValue = integerVal;
    _constantType = eInteger;
}

Constant::Constant(double floatVal) : Expression(Node::eConstant) {
    value.floatValue = floatVal;
    _constantType = eFloat;
}

Constant::Constant(bool boolVal) : Expression(Node::eConstant) {
    value.boolValue = boolVal;
    _constantType = eBoolean;
}

Constant::ConstantType Constant::getConstantType() const {
    return _constantType;
}

} // namespace ast
} // namespace sugar
