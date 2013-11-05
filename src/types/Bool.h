#ifndef SUGAR_TYPES_BOOL_H
#define SUGAR_TYPES_BOOL_H

#include "BuiltinType.h"

namespace sugar {

namespace ast {
    class Node;
}

namespace types {

class Bool : public BuiltinType
{
public:
    Bool();
    virtual ~Bool();

    virtual void generate(gen::Generation &gen) const;

    static llvm::Value* fnImpl_eq(std::vector<ast::Node*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_notEq(std::vector<ast::Node*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_and(std::vector<ast::Node*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_or(std::vector<ast::Node*> values, gen::Generation &gen);
};

} // namespace types
} // namespace sugar

#endif // SUGAR_TYPES_BOOL_H
