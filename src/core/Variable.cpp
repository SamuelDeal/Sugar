#include "Variable.h"

namespace sugar {
namespace core {

Variable::Variable(const std::string &name, const Type& type, llvm::Value *value) : _type(type) {
    _name = name;
    _value = value;
}

const std::string& Variable::getName() const {
    return _name;
}

const Type& Variable::getType() const {
    return _type;
}

Variable::operator llvm::Value*() const {
    return _value;
}

} // namespace core
} // namespace sugar
