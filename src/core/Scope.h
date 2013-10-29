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

class ScopeType {
public:
    static const unsigned int Logical = 0x0000;
    static const unsigned int Global = 0x0001;
    static const unsigned int Main = 0x0010;
    static const unsigned int Function = 0x0100;
    static const unsigned int Protected = 0x1000;
};

class Scope
{

public:
    Scope(llvm::BasicBlock *block, Scope *parent = NULL, unsigned int type = ScopeType::Logical);
    ~Scope();

    bool isGlobal() const;
    bool isFunction() const;
    bool isVarOwner() const;
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
    unsigned int ifCount;
    unsigned int loopCount;
    core::Scope* getCurrentVarOwnerScope();
    core::Scope* getCurrentFunctionScope();
    std::string toString();
    void replaceBlock(llvm::BasicBlock *block);
    void setReturnReach();
    bool isReturnReach();

protected:
    std::multimap<std::string, Function*> _functions;
    std::multimap<int, Operator*> _operators;
    std::map<std::string, Variable *> _variables;
    std::map<std::string, Type *> _types;
    Scope *_parent;
    llvm::BasicBlock *_block;
    unsigned int _type;
    unsigned int _id;
    bool _returnReach;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_SCOPE_H
