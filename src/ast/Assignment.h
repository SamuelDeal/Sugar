#ifndef SUGAR_AST_ASSIGNMENT_H
#define SUGAR_AST_ASSIGNMENT_H

#include "Statement.h"

namespace sugar {
namespace ast {

class Identifier;
class Expression;

class Assignment : public Statement
{
public:
    Assignment(Identifier *left, Expression *right);
    virtual ~Assignment();
    Identifier *left;
    Expression *right;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_ASSIGNMENT_H
