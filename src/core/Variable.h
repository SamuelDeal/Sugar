#ifndef SUGAR_CORE_VARIABLE_H
#define SUGAR_CORE_VARIABLE_H

#include <string>

namespace llvm {
    class Value;
}

namespace sugar {
namespace core {

class Type;

class Variable
{
public:
    Variable(const std::string &name, const Type& type, llvm::Value *value);

    const std::string& getName() const;
    const Type& getType() const;
    std::string toString() const;

    operator llvm::Value*() const;

protected:
    std::string _name;
    const Type& _type;
    llvm::Value* _value;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_VARIABLE_H
