#ifndef SUGAR_PARSER_INTERACTIVEPARSER_H
#define SUGAR_PARSER_INTERACTIVEPARSER_H

#include <stdio.h>
#include "../utils/config_checked.h"
#include <string>


namespace sugar {

namespace ast {
    class Node;
}

namespace parser {

class ProgramNode;
typedef void (*stmtFunction)(ProgramNode *programStmts, ast::Node *stmt);

class InteractiveParser
{
public:
    InteractiveParser();
    bool parse(FILE *file, const std::string *filename, ProgramNode &programStmts, stmtFunction callback = NULL) const;
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_INTERACTIVEPARSER_H
