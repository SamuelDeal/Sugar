#ifndef SUGAR_CORE_SCOPE_H
#define SUGAR_CORE_SCOPE_H

#include <list>
#include <map>
#include <string>

namespace llvm {
    class BasicBlock;
}

namespace sugar {
namespace core {

class Function;
class Type;
class Variable;
class Operator;

class Scope
{
public:
    Scope(llvm::BasicBlock *block, Scope *parent = NULL, bool forceGlobal = false);
    ~Scope();

    bool isGlobal() const;
    Scope* getParent() const;

    operator llvm::BasicBlock*() const;

    void addFunction(Function *function);
    void addOperator(Operator *op);
    void addVar(Variable *variable);
    void addType(Type *type);
    std::list<Function*> getFuncs(const std::string &name) const;
    std::list<Operator*> getOps(int operatorToken) const;
    Variable* getVar(const std::string &name) const;
    Type* getType(const std::string &name) const;

protected:
    std::multimap<std::string, Function*> _functions;
    std::multimap<int, Operator*> _operators;
    std::map<std::string, Variable *> _variables;
    std::map<std::string, Type *> _types;
    Scope *_parent;
    llvm::BasicBlock *_block;
    bool _forcedGlobal;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_SCOPE_H
