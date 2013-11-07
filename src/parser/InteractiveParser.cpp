#include "InteractiveParser.h"

#ifndef OS_WINDOWS
#include <unistd.h>
#endif

#include "../utils/config_checked.h"
#define INTERACTIVE_INPUT 1
#include "lexer.inter.hpp"



#if LIBREADLINE

#include <readline/readline.h>
#include <readline/history.h>

// Support for the readline and history libraries.  This allows
// nicer input on the interactive part of input.

// Variables to help interface readline with bc.
static char *rl_line = (char *)NULL;
static char *rl_start = (char *)NULL;
static int   rl_len = 0;

// Definitions for readline access.
extern FILE *rl_instream;

// rl_input puts upto MAX characters into BUF with the number put in
// BUF placed in *RESULT.
static void lexer_input (char *buf, int  *result, int max, void* yyscanner) {
    if (yyget_in(yyscanner) != rl_instream) { //not stdin so read as usual
        while ( (*result = read( fileno(yyget_in(yyscanner)), buf, max )) < 0 ) {
            if (errno != EINTR) {
                std::cerr << "read() in flex scanner failed" << std::endl;
                exit (1);
            }
        }
        return;
    }

    if (rl_len == 0) { //Do we need a new string?
        if (rl_start) {
            free(rl_start);
        }
        unsigned short scopes = yyget_extra(yyscanner)->indents.size();
        std::string prompt = "sugar";
        if(yyget_extra(yyscanner)->pendingEndInstr){
            prompt += "*";
        }
        if(scopes > 1){
            prompt += "("+std::to_string(scopes)+")";
        }
        prompt += "> ";
#if SHELL_USE_COLOR
        prompt = "\x1b[33m"+prompt+"\x1b[0m";
#endif
        rl_start = readline (prompt.c_str());
        if (rl_start == NULL) { //end of file
            *result = 0;
            rl_len = 0;
            return;
        }
        rl_line = rl_start;
        rl_len = strlen (rl_line)+1;
        if (rl_len != 1) {
            add_history (rl_line);
        }
        rl_line[rl_len-1] = '\n';
        fflush (stdout);
    }

    if (rl_len <= max) {
        strncpy (buf, rl_line, rl_len);
        *result = rl_len;
        rl_len = 0;
    }
    else {
        strncpy (buf, rl_line, max);
        *result = max;
        rl_line += max;
        rl_len -= max;
    }
}

#elif defined(LIBEDIT) && LIBEDIT
// !!! NEVER TESTED !!!
/*
// Support for the BSD libedit with history for
//   nicer input on the interactive part of input.

#include <histedit.h>

// Have input call the following function.
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    bcel_input((char *)buf, &result, max_size)

// Variables to help interface editline with bc.
static const char *bcel_line = (char *)NULL;
static int   bcel_len = 0;


// Required to get rid of that ugly ? default prompt!
char * null_prompt (EditLine *el)
{
    return "";
}


// lexer_input puts upto MAX characters into BUF with the number put in
// BUF placed in *RESULT.  If the yy input file is the same as
// stdin, use editline.  Otherwise, just read it.

static void lexer_input (char *buf, int *result, int max){
    if (!edit || yyin != stdin)
    {
        while ( (*result = read( fileno(yyin), buf, max )) < 0 )
            if (errno != EINTR)
            {
                yyerror( "read() in flex scanner failed" );
                exit (1);
            }
        return;
    }

    // Do we need a new string?
    if (bcel_len == 0)
    {
        bcel_line = el_gets(edit, &bcel_len);
        if (bcel_line == NULL) {
            // end of file
            *result = 0;
            bcel_len = 0;
            return;
        }
        if (bcel_len != 0)
            history (hist, &histev, H_ENTER, bcel_line);
        fflush (stdout);
    }

    if (bcel_len <= max)
    {
        strncpy (buf, bcel_line, bcel_len);
        *result = bcel_len;
        bcel_len = 0;
    }
    else
    {
        strncpy (buf, bcel_line, max);
        *result = max;
        bcel_line += max;
        bcel_len -= max;
    }
}*/
#endif

namespace sugar {
namespace parser {

InteractiveParser::InteractiveParser()
{
}

bool InteractiveParser::parse(FILE *file, const std::string *filename, ast::Node &programStmts, stmtFunction callback) const {
    yydebug = DEBUG_PARSER;

    yyscan_t scanner;
    yylex_init(&scanner);

    bool interactive = (stdin == file);
#ifndef OS_WINDOWS
    interactive = interactive && (isatty(fileno(file)) == 1);
#endif
    yyset_debug(DEBUG_LEXER, scanner);

    LexerContext lexerCtx(&programStmts, filename, interactive, callback);
    yylex_init_extra(&lexerCtx, &scanner);
    lexerCtx.scanner = scanner;
    lexerCtx.interactive = interactive;
#ifndef OS_WINDOWS
    if(stdin == file){
        rl_instream = stdin;
    }
#endif

    yy_push_state(lex_state_newline, scanner);
    yyset_in( file, scanner );
    bool succeed = (yyparse(&lexerCtx) == 0);

    yylex_destroy(scanner);
    fclose(file);
    return succeed && (lexerCtx.errorCount == 0);
}

} // namespace parser
} // namespace sugar


