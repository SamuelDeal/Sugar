#ifndef SUGAR_PARSER_INTERACTIVEPARSER_H
#define SUGAR_PARSER_INTERACTIVEPARSER_H

#include <stdio.h>
#include "../config_checked.h"



namespace sugar {

namespace ast {
    class Node;
}

namespace parser {

typedef void (*stmtFunction)(ast::Node *programStmts, ast::Node *stmt);

class InteractiveParser
{
public:
    InteractiveParser();
    void parse(FILE *file, ast::Node &programStmts, stmtFunction callback = NULL) const;

protected:
    void onProgramStmt(ast::Node *programStmts, ast::Node *newStmt);
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_INTERACTIVEPARSER_H
