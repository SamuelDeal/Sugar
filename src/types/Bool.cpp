#include "Bool.h"

#include "../parser/parser.hpp"
#include "../core/Operator.h"


namespace sugar {
namespace types {

Bool::Bool()
{
}

void Bool::generate(gen::Generation &gen) const {
    std::list<const core::Type*> types;
    types.push_back(&gen.boolType);
    types.push_back(&gen.boolType);
    core::Operator *op = new core::Operator(TCEQ, &fnImpl_eq, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.boolType);
    types.push_back(&gen.boolType);
    op = new core::Operator(TCNE, &fnImpl_notEq, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.boolType);
    types.push_back(&gen.boolType);
    op = new core::Operator(TAND, &fnImpl_and, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.boolType);
    types.push_back(&gen.boolType);
    op = new core::Operator(TOR, &fnImpl_or, &gen.boolType, types);
    gen.rootScope.addOperator(op);
}

llvm::Value* Bool::fnImpl_eq(std::vector<llvm::Value*> values, gen::Generation &gen) {
    return gen.builder.CreateICmpEQ(values[0], values[1]);
}

llvm::Value* Bool::fnImpl_notEq(std::vector<llvm::Value*> values, gen::Generation &gen) {
    return gen.builder.CreateICmpNE(values[0], values[1]);
}

llvm::Value* Bool::fnImpl_and(std::vector<llvm::Value*> values, gen::Generation &gen) {
    return gen.builder.CreateAnd(values[0], values[1]);
}

llvm::Value* Bool::fnImpl_or(std::vector<llvm::Value*> values, gen::Generation &gen) {
    return gen.builder.CreateOr(values[0], values[1]);
}

} // namespace types
} // namespace sugar
