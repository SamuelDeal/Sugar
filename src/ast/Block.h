#ifndef SUGAR_AST_BLOCK_H
#define SUGAR_AST_BLOCK_H

#include "Statement.h"
#include <list>

namespace sugar {
namespace ast {

class Block : public Statement
{
public:
    Block();
    virtual ~Block();
    std::list<Statement *> stmts;
    bool isFunction;
    unsigned int ifCount;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_BLOCK_H
