#ifndef SUGAR_PARSER_BATCHPARSER_H
#define SUGAR_PARSER_BATCHPARSER_H

#include <stdio.h>

namespace sugar {

namespace ast {
    class Node;
}

namespace parser {

class BatchParser
{
public:
    BatchParser();
    void parse(FILE *file, ast::Node &programStmts) const;
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_BATCHPARSER_H


