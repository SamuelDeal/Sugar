#include <iostream>
#include <stdio.h>
#include "codegen.h"
#include "node.h"
#include "functionlist.h"
#include "EditLineReader.h"
#include "parser.hpp"


//Unusual, but it's the cleanest way
#define READLINE 1
#include "lexer.cpp"

#if defined(LIBEDIT)
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
char *
null_prompt (EditLine *el)
{
    return "";
}


/* bcel_input puts upto MAX characters into BUF with the number put in
   BUF placed in *RESULT.  If the yy input file is the same as
   stdin, use editline.  Otherwise, just read it.
*/

static void bcel_input (char *buf, int  *result, int   max)
/*bcel_input (buf, result, max)
    char *buf;
    int  *result;
    int   max;*/
{
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

    /* Do we need a new string? */
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

#ifdef READLINE

#include <readline/readline.h>
#include <readline/history.h>

extern void yyerror(const char *s);

/* Support for the readline and history libraries.  This allows
   nicer input on the interactive part of input. */

/* Have input call the following function. */
#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    rl_input((char *)buf, &result, max_size)

/* Variables to help interface readline with bc. */
static char *rl_line = (char *)NULL;
static char *rl_start = (char *)NULL;
static int   rl_len = 0;

/* Definitions for readline access. */
extern FILE *rl_instream;

/* rl_input puts upto MAX characters into BUF with the number put in
   BUF placed in *RESULT.  If the yy input file is the same as
   rl_instream (stdin), use readline.  Otherwise, just read it.
*/

int input_count = 0;
FILE *logFile;

static void rl_input (char *buf, int  *result, int   max)
/*rl_input (buf, result, max)
    char *buf;
    int  *result;
    int   max;*/
{
    //fprintf(stderr, "******* input *******");
    /*if (yyin != rl_instream)
    {
        fprintf(stderr, "******* ! yyin *******");
        while ( (*result = read( fileno(yyin), buf, max )) < 0 )
            if (errno != EINTR)
            {
                yyerror( "read() in flex scanner failed" );
                exit (1);
            }
        return;
    }*/

    /* Do we need a new string? */
    if (rl_len == 0)
    {
        if (rl_start)
            free(rl_start);
        rl_start = readline ("sugar> ");
        if (rl_start == NULL) {
            /* end of file */
            *result = 0;
            rl_len = 0;
            return;
        }
        rl_line = rl_start;
        rl_len = strlen (rl_line)+1;
        if (rl_len != 1)
            add_history (rl_line);
        rl_line[rl_len-1] = ';';
        fflush (stdout);
    }

    /*if(rl_len > 0 && rl_line[0] == '\n'){
        --rl_len;
        ++rl_line;
    }*/

    if (rl_len <= max)
    {
        strncpy (buf, rl_line, rl_len);
        *result = rl_len;
        rl_len = 0;
    }
    else
    {
        strncpy (buf, rl_line, max);
        *result = max;
        rl_line += max;
        rl_len -= max;
    }
    ++input_count;
    fprintf(logFile, "\n***New Input (%d) :\"%s\"\n", input_count, (char*)buf);
    fflush (logFile);
}
#endif



extern NBlock* programBlock;

int main(int argc, char **argv)
{
    logFile = fopen("/home/sam/sugar.log", "w+");
    yydebug = 1;
    rl_readline_name = "sugari";
    InitializeNativeTarget();
    yyparse();
}


void onMainStatement(){

    fprintf(stderr, "************ onMainStatement ***********");
    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();
}
