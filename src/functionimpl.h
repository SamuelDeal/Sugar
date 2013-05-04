#ifndef FUNCTION_H
#define FUNCTION_H

#include <list>
#include <string>

namespace llvm {
class Type;
class Function;
}

namespace core{

class FunctionImpl
{
public:
    FunctionImpl(const std::string &name, llvm::Function *fn, const std::list<llvm::Type *> &types);
    bool match(const std::list<llvm::Type *> &types) const;
    const std::string& getName() const;
    llvm::Function* getLLVMFunction() const;

private:
    llvm::Function *_function;
    std::list<llvm::Type *> _types;
    std::string _name;
};

}
#endif // FUNCTION_H
