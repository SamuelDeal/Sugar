#ifndef SUGAR_PARSER_BATCHPARSER_H
#define SUGAR_PARSER_BATCHPARSER_H

#include <stdio.h>

namespace sugar {

namespace ast {
    class Block;
    class Statement;
}

namespace parser {

class BatchParser
{
public:
    BatchParser();
    void parse(FILE *file, ast::Block &programStmts) const;

protected:
    void onProgramStmt(ast::Statement *stmt);
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_BATCHPARSER_H


