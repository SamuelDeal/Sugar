#ifndef SUGAR_PARSER_INTERACTIVEPARSER_H
#define SUGAR_PARSER_INTERACTIVEPARSER_H

#include <stdio.h>

namespace sugar {

namespace ast {
    class Block;
    class Statement;
}

namespace parser {

typedef void (*stmtFunction)(sugar::ast::Statement *stmt);

class InteractiveParser
{
public:
    InteractiveParser();
    void parse(FILE *file, ast::Block &programStmt, stmtFunction callback = NULL) const;
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_INTERACTIVEPARSER_H
