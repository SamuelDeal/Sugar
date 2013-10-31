#ifndef SUGAR_TYPES_INT_H
#define SUGAR_TYPES_INT_H

#include "BuiltinType.h"

namespace sugar {
namespace types {

class Int : public BuiltinType
{
public:
    Int();

    virtual void generate(gen::Generation &gen) const;

    static llvm::Value* fnImpl_add(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_sub(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_mul(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_eq(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_NotEq(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_less(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_lessOrEq(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_more(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_moreOrEq(std::vector<llvm::Value*> values, gen::Generation &gen);

    static llvm::Value* fnImpl_PreInc(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_PostInc(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_PreDec(std::vector<llvm::Value*> values, gen::Generation &gen);
    static llvm::Value* fnImpl_PostDec(std::vector<llvm::Value*> values, gen::Generation &gen);

    static llvm::Value* fnImpl_intToFloat(std::vector<llvm::Value*> values, gen::Generation &gen);
};

} // namespace types
} // namespace sugar

#endif // SUGAR_TYPES_INT_H
