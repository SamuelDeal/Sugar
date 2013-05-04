#ifndef SUGAR_AST_TYPEIDENTIFIER_H
#define SUGAR_AST_TYPEIDENTIFIER_H

#include <string>

#include "Node.h"

namespace sugar {
namespace ast {

class TypeIdentifier : public Node
{
public:
    TypeIdentifier(std::string *name);
    virtual ~TypeIdentifier();
    std::string *name;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_TYPEIDENTIFIER_H
