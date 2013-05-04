#ifndef SUGAR_AST_VARIABLEDECLARATION_H
#define SUGAR_AST_VARIABLEDECLARATION_H

#include "Statement.h"

#include "../utils.h"

namespace sugar {
namespace ast {

class Identifier;
class TypeIdentifier;
class Expression;

class VariableDeclaration : public Statement
{
public:
    VariableDeclaration(TypeIdentifier *type, Identifier *id, Expression *assign = NULL);
    virtual ~VariableDeclaration();

    TypeIdentifier *type;
    Identifier *id;
    Expression *assign;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_VARIABLEDECLARATION_H
