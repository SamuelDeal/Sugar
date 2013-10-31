#include "FunctionLookupPass.h"

#include "../../config_checked.h"
#include "../../ast/all.h"

namespace sugar {
namespace gen {
namespace pass {

FunctionLookupPass::FunctionLookupPass() {
}

FunctionLookupPass::~FunctionLookupPass() {
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::ArgumentDeclaration *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::Assignment *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::Block *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::Comparison *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::Constant *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::FunctionCall *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::Identifier *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::Operator *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::IfExpression *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen) {
    return NULL;
}

llvm::Value* FunctionLookupPass::parse(ast::Node *node, ast::WhileStmt *data, Generation &gen) {
    return NULL;
}


} // namespace pass
} // namespace gen
} // namespace sugar
