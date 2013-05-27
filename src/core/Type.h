#ifndef SUGAR_CORE_TYPE_H
#define SUGAR_CORE_TYPE_H


#include <string>

namespace llvm {
    class Type;
}

namespace sugar {
namespace core {

class Type
{
public:
    Type(const std::string &name, llvm::Type *type);

    const std::string& getName() const;
    operator llvm::Type*() const;
    bool operator==(const Type &other) const;
    bool operator!=(const Type &other) const;

protected:
    llvm::Type *_llvmType;
    std::string _name;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_TYPE_H
