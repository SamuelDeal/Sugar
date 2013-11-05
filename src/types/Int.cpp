#include "Int.h"

#include "../parser/parser.hpp"
#include "../core/Operator.h"
#include "../core/Cast.h"
#include "../ast/Node.h"

namespace sugar {
namespace types {

Int::Int()
{
}

Int::~Int() {
}

void Int::generate(gen::Generation &gen) const {
    std::list<const core::Type*> types;
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    core::Operator *op = new core::Operator(TPLUS, &fnImpl_add, &gen.intType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new core::Operator(TMINUS, &fnImpl_sub, &gen.intType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new core::Operator(TMUL, &fnImpl_mul, &gen.intType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new core::Operator(TCEQ, &fnImpl_eq, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new core::Operator(TCNE, &fnImpl_NotEq, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new core::Operator(TCLT, &fnImpl_less, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new core::Operator(TCLE, &fnImpl_lessOrEq, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new core::Operator(TCGT, &fnImpl_more, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    types.clear();
    types.push_back(&gen.intType);
    types.push_back(&gen.intType);
    op = new core::Operator(TCGE, &fnImpl_moreOrEq, &gen.boolType, types);
    gen.rootScope.addOperator(op);

    op = new core::Operator(TPLUSPLUS, &fnImpl_PreInc, &gen.intType, &gen.intType, true);
    gen.rootScope.addOperator(op);

    op = new core::Operator(TPLUSPLUS, &fnImpl_PostInc, &gen.intType, &gen.intType, false);
    gen.rootScope.addOperator(op);

    op = new core::Operator(TMINUSMINUS, &fnImpl_PreDec, &gen.intType, &gen.intType, true);
    gen.rootScope.addOperator(op);

    op = new core::Operator(TMINUSMINUS, &fnImpl_PostDec, &gen.intType, &gen.intType, false);
    gen.rootScope.addOperator(op);

    gen.castGraph.addImplicitCast(new core::Cast(&fnImpl_intToFloat, &gen.floatType, &gen.intType));
}

llvm::Value* Int::fnImpl_add(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateAdd(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_sub(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateSub(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_mul(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateMul(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_eq(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateICmpEQ(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_NotEq(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateICmpNE(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_less(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateICmpSLT(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_lessOrEq(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateICmpSLE(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_more(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateICmpSGT(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_moreOrEq(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateICmpSGE(values[0]->getValue(), values[1]->getValue());
}

llvm::Value* Int::fnImpl_intToFloat(std::vector<ast::Node*> values, gen::Generation &gen) {
    return gen.builder.CreateFPToSI(values[0]->getValue(), gen.floatType);
}

llvm::Value* Int::fnImpl_PreInc(std::vector<ast::Node*> values, gen::Generation &gen) {
    llvm::Value* result = gen.builder.CreateAdd(values[0]->getValue(), llvm::ConstantInt::get(gen.intType, 1, true));
    llvm::StoreInst *assign = gen.builder.CreateStore(result, values[0]->getRef());
    return result;
}

llvm::Value* Int::fnImpl_PostInc(std::vector<ast::Node*> values, gen::Generation &gen) {
    //llvm::AllocaInst *tmpValue = gen.builder.CreateAlloca(gen.intType);
    //llvm::StoreInst *assignTmp = gen.builder.CreateStore(values[0]->getValue(), tmpValue);

    llvm::Value* result = gen.builder.CreateAdd(values[0]->getValue(), llvm::ConstantInt::get(gen.intType, 1, true));
    gen.builder.CreateStore(result, values[0]->getRef());

    return values[0]->getValue();
}

llvm::Value* Int::fnImpl_PreDec(std::vector<ast::Node*> values, gen::Generation &gen) {
    llvm::Value* result = gen.builder.CreateSub(values[0]->getValue(), llvm::ConstantInt::get(gen.intType, 1, true));
    llvm::StoreInst *assign = gen.builder.CreateStore(result, values[0]->getRef());
    return result;
}

llvm::Value* Int::fnImpl_PostDec(std::vector<ast::Node*> values, gen::Generation &gen) {
    //llvm::AllocaInst *tmpValue = gen.builder.CreateAlloca(gen.intType);
    //llvm::StoreInst *assignTmp = gen.builder.CreateStore(values[0]->getValue(), tmpValue);

    llvm::Value* result = gen.builder.CreateSub(values[0]->getValue(), llvm::ConstantInt::get(gen.intType, 1, true));
    gen.builder.CreateStore(result, values[0]->getRef());

    return values[0]->getValue();
}

} // namespace types
} // namespace sugar
