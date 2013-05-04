#ifndef SUGAR_AST_RETURNSTMT_H
#define SUGAR_AST_RETURNSTMT_H

#include "Statement.h"
#include "Expression.h"

namespace sugar {
namespace ast {

class ReturnStmt : public Statement
{
public:
    ReturnStmt(Expression *expression);
    virtual ~ReturnStmt();
    Expression* expression;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_RETURNSTMT_H
