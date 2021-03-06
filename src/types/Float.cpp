#include "Float.h"

#include "../parser/parser.hpp"
#include "../core/Operator.h"
#include "../ast/Node.h"

namespace sugar {
namespace types {

Float::Float()
{
}

Float::~Float() {
}

void Float::generate(gen::Generation &gen) const {
    std::list<const core::Type*> types;
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    core::Operator *op = new core::Operator(TPLUS, &fnImpl_add, &gen.floatType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new core::Operator(TMINUS, &fnImpl_sub, &gen.floatType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new core::Operator(TMUL, &fnImpl_mul, &gen.floatType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new core::Operator(TDIV, &fnImpl_div, &gen.floatType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new core::Operator(TCLT, &fnImpl_less, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new core::Operator(TCLE, &fnImpl_lessOrEq, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new core::Operator(TCGT, &fnImpl_more, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.floatType);
    types.push_back(&gen.floatType);
    op = new core::Operator(TCGE, &fnImpl_moreOrEq, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    op = new core::Operator(TPLUSPLUS, &fnImpl_PreInc, &gen.floatType, &gen.floatType, true);
    gen.rootScope.addOperator(op);

    op = new core::Operator(TPLUSPLUS, &fnImpl_PostInc, &gen.floatType, &gen.floatType, false);
    gen.rootScope.addOperator(op);

    op = new core::Operator(TMINUSMINUS, &fnImpl_PreDec, &gen.floatType, &gen.floatType, true);
    gen.rootScope.addOperator(op);

    op = new core::Operator(TMINUSMINUS, &fnImpl_PostDec, &gen.floatType, &gen.floatType, false);
    gen.rootScope.addOperator(op);
}

llvm::Value* Float::fnImpl_add(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFAdd(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Float::fnImpl_sub(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFSub(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Float::fnImpl_mul(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFMul(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Float::fnImpl_div(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFDiv(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Float::fnImpl_less(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFCmpULT(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Float::fnImpl_lessOrEq(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFCmpULE(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Float::fnImpl_more(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFCmpUGT(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Float::fnImpl_moreOrEq(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFCmpUGE(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Float::fnImpl_PreInc(std::vector<ast::Node*> values, gen::Generation &gen) {
    llvm::Value* result = gen.builder.CreateAdd(values[0]->getValue(), llvm::ConstantInt::get(gen.floatType, 1, true));
    llvm::StoreInst *assign = gen.builder.CreateStore(result, values[0]->getRef());
    return result;
}

llvm::Value* Float::fnImpl_PostInc(std::vector<ast::Node*> values, gen::Generation &gen) {
    llvm::Value* result = gen.builder.CreateAdd(values[0]->getValue(), llvm::ConstantInt::get(gen.floatType, 1, true));
    gen.builder.CreateStore(result, values[0]->getRef());

    return values[0]->getValue();
}

llvm::Value* Float::fnImpl_PreDec(std::vector<ast::Node*> values, gen::Generation &gen) {
    llvm::Value* result = gen.builder.CreateSub(values[0]->getValue(), llvm::ConstantInt::get(gen.floatType, 1, true));
    llvm::StoreInst *assign = gen.builder.CreateStore(result, values[0]->getRef());
    return result;
}

llvm::Value* Float::fnImpl_PostDec(std::vector<ast::Node*> values, gen::Generation &gen) {
    llvm::Value* result = gen.builder.CreateSub(values[0]->getValue(), llvm::ConstantInt::get(gen.floatType, 1, true));
    gen.builder.CreateStore(result, values[0]->getRef());

    return values[0]->getValue();
}


} // namespace types
} // namespace sugar
