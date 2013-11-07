#include "BatchParser.h"

#include "../config_checked.h"
#define INTERACTIVE_INPUT 0
#include "lexer.batch.hpp"

#include "../ast/Block.h"

namespace sugar {
namespace parser {

BatchParser::BatchParser()
{
}

void BatchParser_onProgramStmt(ast::Node *programStmts, ast::Node *newStmt){
    ((ast::Block *)(programStmts->data))->stmts->push_back(newStmt);
}

bool BatchParser::parse(FILE *file, const std::string *filename, ast::Node &programStmts) const {
    yydebug = DEBUG_PARSER;

    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_debug(DEBUG_LEXER, scanner);

    LexerContext lexerCtx(&programStmts, filename, false, BatchParser_onProgramStmt);
    yylex_init_extra(&lexerCtx, &scanner);
    lexerCtx.scanner = scanner;

    yy_push_state(lex_state_newline, scanner);
    yyset_in( file, scanner );
    bool succeed = (yyparse(&lexerCtx) == 0);

    yylex_destroy(scanner);
    fclose(file);
    return succeed && (lexerCtx.errorCount == 0);
}



} // namespace parser
} // namespace sugar
