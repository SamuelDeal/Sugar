#include "FunctionLookupPass.h"

#include "../../config_checked.h"
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

void FunctionLookupPass::parse(ast::Node *node, ast::ArgumentDeclaration *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::Assignment *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::Block *data, Generation &gen) {
    std::list<ast::Node*>::const_iterator it;
    for (it = data->stmts->begin(); it != data->stmts->end() && !gen.scope->isReturnReach(); it++) {
        parseNode(*it, gen);
    }
}

void FunctionLookupPass::parse(ast::Node *node, ast::Comparison *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::Constant *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::FunctionCall *data, Generation &gen) {
    return;
}

/*void FunctionLookupPass::parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen) {
    return;
}*/

void FunctionLookupPass::parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen) {
    Type *returnType = gen.scope->getType(*data->getType()->name);
    if(NULL == returnType){
        std::cout << "Unknown type " << *data->getType()->name << std::endl;
        return;
    }

    std::list<ast::Node*>::const_iterator it;
    std::list<const Type*> types;
    for (it = data->arguments->begin(); it != data->arguments->end(); it++) {
        Type *argType = gen.scope->getType(*((ast::ArgumentDeclaration*)(*it)->data)->getType()->name);
        if(argType == NULL){
            std::cout << "Unknown type " << *((ast::ArgumentDeclaration*)(*it)->data)->getType()->name << std::endl;
            return;
        }
        types.push_back(argType);
    }


    Function *fn = new Function(*data->getId()->name, gen.scope->getType(*data->getType()->name), types, node, [=, &gen] () -> llvm::Function* {
        Type *returnType = gen.scope->getType(*data->getType()->name);
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
    /*
#if DEBUG_GENERATOR
    std::cerr << "Creating function: " << *data->getId()->name << std::endl;
#endif
    //node->setValue(function);
    */
}

void FunctionLookupPass::parse(ast::Node *node, ast::FunctionImplementation *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::Identifier *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::Operator *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::IfExpression *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen) {
    return;
}

void FunctionLookupPass::parse(ast::Node *node, ast::WhileStmt *data, Generation &gen) {
    return;
}


} // namespace pass
} // namespace gen
} // namespace sugar
