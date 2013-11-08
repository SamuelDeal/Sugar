#ifndef SUGAR_PARSER_PROGRAMNODE_H
#define SUGAR_PARSER_PROGRAMNODE_H

#include "../ast/Node.h"

namespace sugar {
namespace parser {

class ProgramNode: public ast::Node
{
public:
    ProgramNode(const std::string *filename);
    virtual ~ProgramNode();

    void addStmt(ast::Node *stmt);
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_PROGRAMNODE_H
