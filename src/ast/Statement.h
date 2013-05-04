#ifndef SUGAR_AST_STATEMENT_H
#define SUGAR_AST_STATEMENT_H

#include "Node.h"

namespace sugar {
namespace ast {

class Statement : public Node
{
protected:
    Statement(Kind kind);
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_STATEMENT_H
