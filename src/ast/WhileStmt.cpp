#include "WhileStmt.h"

#include "Expression.h"
#include "Block.h"

namespace sugar {
namespace ast {

WhileStmt::WhileStmt(Expression *exprArg, Block *blockArg, bool testFirstArg): Statement(Node::eWhileStmt)
{
    cond = exprArg;
    block = blockArg;
    testFirst = testFirstArg;
}

WhileStmt::~WhileStmt()
{
    delete cond;
    delete block;
}

} // namespace ast
} // namespace sugar
