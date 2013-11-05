#ifndef SUGAR_AST_IDENTIFIER_H
#define SUGAR_AST_IDENTIFIER_H

#include <string>
#include "NodeData.h"
#include "../parser/parser.hpp"

namespace sugar {
namespace ast {

class Node;

class Identifier : public NodeData
{
public:
    static Node* create(std::string *name, YYLTYPE yyloc);

    Identifier(std::string *name);
    virtual ~Identifier();

    std::string *name;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_IDENTIFIER_H
