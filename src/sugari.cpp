#include <iostream>
#include <stdio.h>

#include "config.h"
#define INTERACTIVE_INPUT 1
#include "lexer.hpp"

#include "gen/Interpreter.h"
#include "gen/GeneratedCode.h"

#if defined(LIBREADLINE) && LIBREADLINE

#include <readline/readline.h>
#include <readline/history.h>

extern void yyerror(const char *s);

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
static void lexer_input (char *buf, int  *result, int max) {
    if (yyin != rl_instream) { //not stdin so read as usual
        while ( (*result = read( fileno(yyin), buf, max )) < 0 ) {
            if (errno != EINTR) {
                yyerror( "read() in flex scanner failed" );
                exit (1);
            }
        }
        return;
    }

    if (rl_len == 0) { //Do we need a new string?
        if (rl_start) {
            free(rl_start);
        }
        rl_start = readline ("sugar> ");
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
/* Support for the BSD libedit with history for
   nicer input on the interactive part of input. */

#include <histedit.h>

/* Have input call the following function. */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    bcel_input((char *)buf, &result, max_size)

/* Variables to help interface editline with bc. */
static const char *bcel_line = (char *)NULL;
static int   bcel_len = 0;


/* Required to get rid of that ugly ? default prompt! */
char * null_prompt (EditLine *el)
{
    return "";
}


/* lexer_input puts upto MAX characters into BUF with the number put in
   BUF placed in *RESULT.  If the yy input file is the same as
   stdin, use editline.  Otherwise, just read it.
*/

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
            /* end of file */
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
}
#endif

using namespace std;
using namespace sugar;

ast::Block* programBlock = new ast::Block();
gen::Interpreter *interpreter;
gen::GeneratedCode *generated;

int main(int argc, char **argv)
{
    yy_flex_debug = DEBUG_LEXER;
    yydebug = DEBUG_PARSER;
    rl_instream = stdin;
    if(argc > 1){
        yyin = fopen(argv[1],"r");
        if(yyin == NULL){
            std::cout << "Unable to open " << argv[1] << std::endl;
            return 1;
        }
    }

    interpreter = new gen::Interpreter();
    generated = interpreter->generate(programBlock);

    yyparse();
    return 0;
}

void onMainStatement(sugar::ast::Statement *stmt){
    if(stmt->getKind() == ast::Node::eFunctionDeclaration){
        programBlock->stmts.push_back(stmt);
    }
    else if(stmt->getKind() == ast::Node::eVariableDeclaration){
        ast::VariableDeclaration *stmtVar = (ast::VariableDeclaration*)stmt;
        if(stmtVar->assign == NULL){
            programBlock->stmts.push_back(stmt);
        }
        else{ //split assign and declaration
            ast::Assignment *assign = new ast::Assignment(stmtVar->id, stmtVar->assign);
            stmtVar->assign = NULL;
            programBlock->stmts.push_back(stmtVar);
            interpreter->run(assign, generated);
        }
    }
    else {
        interpreter->run(stmt, generated);
    }
}
