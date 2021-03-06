#include "FunctionLookupPass.h"

#include "../../utils/config_checked.h"
#include "../../ast/all.h"

#include "../Generation.h"

namespace sugar {

using namespace core;

namespace gen {
namespace pass {

FunctionLookupPass::FunctionLookupPass() {
}

FunctionLookupPass::~FunctionLookupPass() {
}

bool FunctionLookupPass::parse(ast::Node *node, ast::ArgumentDeclaration *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::Assignment *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::Block *data, Generation &gen) {
    std::list<ast::Node*>::const_iterator it;
    bool succeed = true;
    for (it = data->stmts->begin(); it != data->stmts->end() && !gen.scope->isReturnReach(); it++) {
        if(!parseNode(*it, gen)){
            succeed = false;
            if(gen.maxErrorCountReached()){
                return false;
            }
        }
    }
    return succeed;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::Comparison *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::Constant *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::FunctionCall *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen) {
    Type *returnType = gen.scope->getType(*data->getType()->name);
    if(NULL == returnType){
        gen.addError("unknown type " + *data->getType()->name, &data->type->yylloc);
        gen.addError("invalid function declaration", &node->yylloc);
        return false;
    }

    std::list<ast::Node*>::const_iterator it;
    std::list<const Type*> types;
    for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
        std::string *typeName = ((ast::ArgumentDeclaration*)(*it)->data)->getType()->name;
        Type *argType = gen.scope->getType(*typeName);
        if(argType == NULL){
            gen.addError("unknown type " + *typeName, &((ast::ArgumentDeclaration*)(*it)->data)->type->yylloc);
            gen.addError("invalid function declaration", &node->yylloc);
            return false;
        }
        types.push_back(argType);
    }

    Function *fn = new Function(*data->getId()->name, gen.scope->getType(*data->getType()->name), types, node, [=, &gen] () -> llvm::Function* {
        std::vector<llvm::Type*> argTypes;
        std::list<ast::Node*>::const_iterator it;
        for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
            Type *argType = gen.scope->getType(*((ast::ArgumentDeclaration*)(*it)->data)->getType()->name);
            argTypes.push_back(*argType);
        }

        llvm::FunctionType *ftype = llvm::FunctionType::get(*returnType, llvm::makeArrayRef(argTypes), false);
        llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, data->getId()->name->c_str(), gen.module);
        return function;
    });

    gen.scope->addFunction(fn);
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::FunctionImplementation *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::Identifier *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::Operator *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::IfExpression *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen) {
    return true;
}

bool FunctionLookupPass::parse(ast::Node *node, ast::WhileStmt *data, Generation &gen) {
    return true;
}


} // namespace pass
} // namespace gen
} // namespace sugar
