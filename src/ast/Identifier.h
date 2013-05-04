#ifndef SUGAR_AST_IDENTIFIER_H
#define SUGAR_AST_IDENTIFIER_H

#include <string>
#include "Expression.h"

namespace sugar {
namespace ast {

class Identifier : public Expression
{
public:
    Identifier(std::string *name);
    virtual ~Identifier();
    std::string *name;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_IDENTIFIER_H
