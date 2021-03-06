#ifndef SUGAR_PARSER_BATCHPARSER_H
#define SUGAR_PARSER_BATCHPARSER_H

#include <stdio.h>
#include <string>

namespace sugar {

namespace ast {
    class Node;
}

namespace parser {

class ProgramNode;

class BatchParser
{
public:
    BatchParser();
    bool parse(FILE *file, const std::string *filename, ProgramNode &programStmts) const;
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_BATCHPARSER_H


