#ifndef SUGAR_AST_EXPRESSION_H
#define SUGAR_AST_EXPRESSION_H

#include "Node.h"

namespace sugar {
namespace ast {

class Expression : public Node
{
protected:
    Expression(Kind kind);
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_EXPRESSION_H
