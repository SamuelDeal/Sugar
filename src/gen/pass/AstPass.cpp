#include "AstPass.h"

#include <algorithm>

#include "../../parser/parser.hpp"
#include "../../gen/Generation.h"
/*

namespace sugar {

namespace ast {
    class Node;
    class Assignment;
    class Block;
    class Comparison;
    class Constant;
    class ExpressionStmt;
    class FunctionCall;
    class FunctionDeclaration;
    class Identifier;
    class IfExpression;
    class ReturnStmt;
    class VariableDeclaration;
    class TypeIdentifier;
    class Operator;
    class WhileStmt;
}

namespace gen {
namespace pass {


AstPass::AstPass()
{
}

AstPass::~AstPass(){

}


llvm::Value* AstPass::parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen)  {
    //Nothing to do
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::Assignment *data, Generation &gen)  {
    parseNode(data->right, gen);
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::Block *data, Generation &gen)  {
    std::list<ast::Node*>::const_iterator it;
    for (it = data->stmts.begin(); it != data->stmts.end() && !gen.scope->isReturnReach(); it++) {
        parseNode(*it, gen);
    }
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::Constant *data, Generation &gen)  {
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::FunctionCall *data, Generation &gen)  {
    std::list<ast::Node*>::const_iterator it;
    for (it = data->args->begin(); it != data->args->end(); it++) {
        parseNode(*it, gen);
    }
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::Operator *data, Generation &gen)  {
    if(data->operatorId == TPLUSPLUS && data->before){
        ast::Block *block = new ast::Block();

        ast::Node *tmpVar = ast::VariableDeclaration::create(
                    ast::TypeIdentifier::create(data->args->front()), NULL, data->args->front());
        block->stmts.push_back(tmpVar);

        std::list<ast::Node*> *args = new std::list<ast::Node*>();
        args->push_back(data->args->front());
        args->push_back(ast::Constant::create((long long)1));
        block->stmts.push_back(ast::Assignment::create(data->args->front(), ast::Operator::create(TPLUS, args)));

        node->reset(ast::Node::eBlock, block);
        delete data;
        return parseNode(node, gen);
    }
    else if(data->operatorId == TPLUSPLUS && !data->before){
        std::list<ast::Node*> *args = new std::list<ast::Node*>();
        args->push_back(data->args->front());
        args->push_back(ast::Constant::create((long long)1));
        node->reset(ast::Node::eAssignment, new ast::Assignment(data->args->front(), ast::Operator::create(TPLUS, args)));
        delete data;
        return parseNode(node, gen);
    }
    else if(data->operatorId == TMINUSMINUS && data->before){
        ast::Block *block = new ast::Block();

        ast::VariableDeclaration *tmpVar = ast::VariableDeclaration::create(
                    ast::TypeIdentifier::create(data->args->front()), NULL, data->args->front());
        block->stmts.push_back(tmpVar);

        std::list<ast::Node*> *args = new std::list<ast::Node*>();
        args->push_back(data->args->front());
        args->push_back(ast::Constant::create((long long)1));
        block->stmts.push_back(ast::Assignment::create(data->args->front(), ast::Operator::create(TMINUS, args)));

        node->reset(ast::Node::eBlock, block);
        delete data;
        return parseNode(node, gen);
    }
    else if(data->operatorId == TMINUSMINUS && !data->before){
        std::list<ast::Node*> *args = new std::list<ast::Node*>();
        args->push_back(data->args->front());
        args->push_back(ast::Constant::create((long long)1));
        node->reset(ast::Node::eAssignment, new ast::Assignment(data->args->front(), ast::Operator::create(TMINUS, args)));
        delete data;
        return parseNode(node, gen);
    }
    else{
        std::list<ast::Node*>::const_iterator it;
        for (it = node->data->args->begin(); it != node->data->args->end(); it++) {
            parseNode(*it, gen);
        }
        return NULL;
    }
}

llvm::Value* AstPass::parse(ast::Node *node, ast::Identifier *data, Generation &gen)  {
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen)  {
    /*Type *returnType = gen.scope->getType(*node->type->name);
    if(NULL == returnType){
        std::cout << "Unknown type " << *node->type->name << std::endl;
        return NULL;
    }

    std::vector<llvm::Type*> argTypes;
    std::list<ast::VariableDeclaration *>::const_iterator it;
    for (it = node->arguments->begin(); it != node->arguments->end(); it++) {
        Type *argType = gen.scope->getType(*(*it)->type->name);
        if(argType == NULL){
            std::cout << "Unknown type " << *(*it)->type->name << std::endl;
            return NULL;
        }
        argTypes.push_back(*argType);
    }
    llvm::FunctionType *ftype = llvm::FunctionType::get(*returnType, llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, node->id->name->c_str(), gen.module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(gen.context, "entry", function, 0);

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;
    std::list<const Type *> types;

    gen.pushBlock(bblock, ScopeType::Function);

    for (it = node->arguments->begin(); it != node->arguments->end(); it++) {
        parse(*it, gen);
        argumentValue = argsValues++;
        argumentValue->setName((*it)->id->name->c_str());
        gen.builder.CreateStore(argumentValue, *gen.scope->getVar(*(*it)->id->name));
        types.push_back((*it)->getType());
    }

    llvm::Value *blockReturnValue = parse(node->block, gen);
    if(node->block->getType() != returnType){
        std::cout << "Function type mismatch with declaration !"  << std::endl;
        return NULL;
    }

    if(*returnType != gen.voidType){
        gen.builder.CreateRet(blockReturnValue);
    }
    else{
        gen.builder.CreateRetVoid();
    }

    gen.popBlock();
    gen.scope->addFunction(new Function(*node->id->name, function, gen.scope->getType(*node->type->name), types));
#if DEBUG_GENERATOR
    std::cerr << "Creating function: " << *node->id->name << std::endl;
#endif
    return function;*/ /*

    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen)  {
    if(data->assign == NULL){
        return NULL;
    }

    ast::Block *block = new ast::Block();
    block->stmts.push_back(new ast::Node(ast::Node::eVariableDeclaration, data));
    block->stmts.push_back(ast::Assignment::create(data->id, data->assign));
    data->assign = NULL;
    node->reset(ast::Node::eBlock, block);
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::IfExpression *data, Generation &gen)  {
    parseNode(data->cond, gen);
    parseNode(data->thenBlk, gen);
    if(data->elseBlk != NULL){
        parseNode(data->elseBlk, gen);
    }
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::Comparison *data, Generation &gen)  {
    std::list<ast::Node *>::iterator it = data->expressions.begin();
    for(++it; it != data->expressions.end(); it++){
        parseNode(*it, gen);
    }
    return NULL;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen)  {
    llvm::Value* expr = parseNode(node->expression, gen);
    llvm::Value* returnExpr = gen.builder.CreateRet(expr);
    gen.scope->setReturnReach();
    return returnExpr;
}

llvm::Value* AstPass::parse(ast::Node *node, ast::WhileStmt *data, Generation &gen)  {
    parseNode(data->cond, gen);
    parseNode(data->block, gen);
    return NULL;
}


} // namespace pass
} // namespace gen
} // namespace sugar

*/
