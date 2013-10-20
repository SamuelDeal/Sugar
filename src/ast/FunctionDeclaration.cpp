#include "FunctionDeclaration.h"

#include "VariableDeclaration.h"
#include "Identifier.h"
#include "TypeIdentifier.h"
#include "Block.h"

namespace sugar {
namespace ast {

FunctionDeclaration::FunctionDeclaration(TypeIdentifier *type, Identifier *id, std::list<VariableDeclaration *> *arguments, Block *block) :
    Statement(Node::eFunctionDeclaration)
{
    this->type = type;
    this->block = block;
    if(block != NULL){
        block->isFunction = true;
    }
    this->id = id;
    this->arguments = arguments;
}

FunctionDeclaration::~FunctionDeclaration(){
    for(std::list<VariableDeclaration *>::iterator i = arguments->begin(); i != arguments->end(); i++){
        delete (*i);
    }
    delete type;
    delete block;
    delete id;
    delete arguments;
}

} // namespace ast
} // namespace sugar
