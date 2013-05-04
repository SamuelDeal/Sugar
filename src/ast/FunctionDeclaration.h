#ifndef SUGAR_AST_FUNCTIONDECLARATION_H
#define SUGAR_AST_FUNCTIONDECLARATION_H

#include "Statement.h"
#include "../core/Function.h"

namespace sugar {
namespace ast {

class VariableDeclaration;
class Identifier;
class TypeIdentifier;
class Block;

class FunctionDeclaration : public Statement
{
public:
    FunctionDeclaration(TypeIdentifier *type, Identifier *id, std::list<VariableDeclaration *> *arguments, Block *block);
    virtual ~FunctionDeclaration();

    TypeIdentifier *type;
    Identifier *id;
    std::list<VariableDeclaration *> *arguments;
    Block *block;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_FUNCTIONDECLARATION_H
