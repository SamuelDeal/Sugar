#include "BatchParser.h"

#include "../config.h"
#define INTERACTIVE_INPUT 0
#include "lexer.batch.hpp"

namespace sugar {
namespace parser {

BatchParser::BatchParser()
{
}

void BatchParser::parse(FILE *file, ast::Block &programStmts) const {
    //yy_flex_debug = DEBUG_LEXER;
    yydebug = DEBUG_PARSER;

    yyscan_t scanner;
    yylex_init(&scanner);

    LexerContext lexerCtx(&programStmts, false);
    yylex_init_extra(&lexerCtx, &scanner);
    lexerCtx.scanner = scanner;

    yy_push_state(lex_state_newline, scanner);
    yyset_in( file, scanner );
    yyparse(&lexerCtx);

    yylex_destroy(scanner);
    fclose(file);
}

} // namespace parser
} // namespace sugar
