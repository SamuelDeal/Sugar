#include <llvm/Support/raw_ostream.h>
#include <llvm/Value.h>

#include "Variable.h"
#include "../utils.h"

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

std::string Variable::toString() const {
    std::string str;
    llvm::raw_string_ostream os(str);
    _value->print(os);
    return os.str();
}

} // namespace core
} // namespace sugar
