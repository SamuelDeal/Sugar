#ifndef SUGAR_TYPES_BOOL_H
#define SUGAR_TYPES_BOOL_H

#include "BuiltinType.h"



namespace sugar {
namespace types {

class Bool : public BuiltinType
{
public:
    Bool();

    virtual void generate(gen::Generation &gen) const;

    static llvm::Value* fnImpl_eq(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_notEq(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_and(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_or(std::vector<llvm::Value*> values, gen::Generation &gen);
};

} // namespace types
} // namespace sugar

#endif // SUGAR_TYPES_BOOL_H
