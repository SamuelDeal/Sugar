#ifndef FUNCTIONLIST_H
#define FUNCTIONLIST_H

#include <list>
#include <string>

namespace llvm{
class Function;
class Type;
}

namespace core {

class FunctionImpl;

class FunctionList
{
public:
    FunctionList();
    void addFunction(const std::string &name, llvm::Function *function, const std::list<llvm::Type *> &types);
    llvm::Function* getFunction(const std::string &name, const std::list<llvm::Type *> &types) const;

private:
    std::list<FunctionImpl *> _functions;
};

extern FunctionList functionList;
}
#endif // FUNCTIONLIST_H
