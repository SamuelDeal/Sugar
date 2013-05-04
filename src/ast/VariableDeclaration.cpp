#include "VariableDeclaration.h"

#include "Identifier.h"
#include "TypeIdentifier.h"
#include "Expression.h"

namespace sugar {
namespace ast {

VariableDeclaration::VariableDeclaration(TypeIdentifier *type, Identifier *id, Expression *assign) : Statement(Node::eVariableDeclaration)
{
    this->type = type;
    this->id = id;
    this->assign = assign;
}

VariableDeclaration::~VariableDeclaration(){
    delete type;
    delete id;
    if(assign != NULL){
        delete assign;
    }
}

} // namespace ast
} // namespace sugar
