#ifndef SUGAR_AST_WHILESTMT_H
#define SUGAR_AST_WHILESTMT_H

#include "Statement.h"

namespace sugar {
namespace ast {

class Expression;
class Block;

class WhileStmt : public Statement
{
public:
    WhileStmt(Expression *expr, Block *block, bool testFirst = false);
    virtual ~WhileStmt();

    Expression *cond;
    Block *block;
    bool testFirst;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_WHILESTMT_H
