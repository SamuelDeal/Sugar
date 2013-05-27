#include "Type.h"

namespace sugar {
namespace core {

Type::Type(const std::string &name, llvm::Type *type){
    _llvmType = type;
    _name = name;
}

Type::operator llvm::Type*() const{
    return _llvmType;
}

const std::string& Type::getName() const{
    return _name;
}

bool Type::operator==(const Type &other) const{
    return _llvmType  == other._llvmType;
}

bool Type::operator!=(const Type &other) const{
    return _llvmType != other._llvmType;
}

} // namespace core
} // namespace sugar
