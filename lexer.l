%{
#include <string>
#include <iostream>
#include <string.h>
#include <queue>
#include <stdarg.h>
#include <stack>

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
#define YYLTYPE YYLTYPE
typedef struct YYLTYPE
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    char *filename;
} YYLTYPE;
#define yyltype YYLTYPE /* obsolescent; will be withdrawn */
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1
#endif


/* globals to track current indentation */
#define INDENT_ALLOW_MIXED 0
#define INDENT_FAIL_ON_ERROR 0
struct ContextIndentInfo {
    enum EStyle {
        UNDEFINED,
        SPACE,
        TAB
    };

    int spaceSize;
    unsigned int indentCount;
    EStyle style;
};
ContextIndentInfo g_indent_context;
unsigned int g_indent_count = 0;
bool g_indent_line_emitted = false;

extern const char* g_current_filename;
bool yySkipNL = true;
std::stack<bool> yyIndentSensitiveStack;
std::queue<int> yyTokenQueue;
bool yySeachLParent = false;

bool yyIndentSensitive(){
    return yyIndentSensitiveStack.empty() ? false : yyIndentSensitiveStack.top();
}

void yyQueueTokens(int count, ...){
    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++){
        yyTokenQueue.push(va_arg(ap, int));
    }
    va_end(ap);
}

/* Don't mangle yylex please! */
#define YY_DECL extern "C" int yylex()

#define YY_USER_INIT { \
        g_indent_context.spaceSize = -1; \
        g_indent_context.indentCount = 0; \
        g_indent_context.style = ContextIndentInfo::UNDEFINED; \
        /*yylloc.first_line = 1;*/ \
        /*yylloc.first_column = 1;*/ \
        BEGIN(initial); \
    }

int yycolumn = 1;
void set_yycolumn(int val) {
    yycolumn = val;
    //yylloc.first_column = yycolumn;
    //yylloc.last_column = yycolumn + yyleng - 1;
}

#define YY_USER_ACTION { \
    /*yylloc.first_line = yylloc.last_line = yylineno;*/ \
    set_yycolumn(yycolumn); \
    yycolumn += yyleng; \
}

void lyyerror(const char *s){
    //fprintf(stderr, "%s: %d:%d: %s: %s\n", g_current_filename, yylineno, yycolumn, s, yytext);
    fprintf(stderr, "%d:%d: %s: %s\n", yylineno, yycolumn, s, yytext);
    exit(1);
}

#include "node.h"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
//extern "C" int yywrap() { }

extern FILE * yyin;

#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)
#define yyReturn(token, ...) yyQueueTokens(VA_NARGS(__VA_ARGS__), __VA_ARGS__); return TOKEN(token);
%}



 /* This is a sub-parser (state) for indentation-sensitive scoping */
%x initial
%x indent
%s normal

 /* %option 8bit reentrant bison-bridge */
%option warn
%option nodefault
%option yylineno
%option noyywrap


 /*%option debug*/
%%
    int indent_caller = normal;
    if(!yyTokenQueue.empty()){
        int result = yyTokenQueue.front();
        yyTokenQueue.pop();
        if(EOF == result){
            yyterminate();
        }
        else{
            return TOKEN(result);
        }
    }

 /* This helps to keep track of the column number.
  * Note that it won't work if you have a rule which includes a newline and is
  * longer than one character because in that case that rule will be favored
  * and this one here won't get called.
  * TL;DR: If you have a rule which includes \n and more, you need to reset
  *        yycolumn inside that rule!
  */
<*>\n {
    yySeachLParent = false;
    set_yycolumn(0);
    yylineno--; REJECT;
}

 /* Everything runs in the <normal> mode and enters the <indent> mode
    when a newline symbol is encountered.
    There is no newline symbol before the first line, so we need to go
    into the <indent> mode by hand there.
 */
<initial>.  {
    yySeachLParent = false;
    set_yycolumn(yycolumn-1);
    indent_caller = normal;
    yyless(0);
    BEGIN(yyIndentSensitive() ? indent : normal);
}
<initial>\n {
    yySeachLParent = false;
    indent_caller = normal;
    yyless(0);
    BEGIN(yyIndentSensitive() ? indent : normal);
}


<normal>[\r\n]+                 {
    yySeachLParent = false;
    g_indent_count = 0;
    g_indent_line_emitted = false;
    indent_caller = YY_START;
    BEGIN(yyIndentSensitive() ? indent : normal);
    if(!yySkipNL){
        yySkipNL = true;
        return TOK_NL;
    }
}
<normal>[ \t]                       { yySeachLParent = false; };
<normal>"return"                    { yySkipNL = false; yySeachLParent = false; return TOKEN(TRETURN); }
<normal>[A-Z][a-zA-Z0-9_\?]*        { yySkipNL = false; yySeachLParent = false; SAVE_TOKEN; return TTYPENAME; }
<normal>[a-z_][a-zA-Z0-9_\?!]*      { yySkipNL = false; yySeachLParent = true; SAVE_TOKEN; return TIDENTIFIER; }
<normal>[0-9]+\.[0-9]*              { yySkipNL = false; yySeachLParent = false; SAVE_TOKEN; return TDOUBLE; }
<normal>[0-9]+                      { yySkipNL = false; yySeachLParent = false; SAVE_TOKEN; return TINTEGER; }
<normal>"="                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TEQUAL); }
<normal>"=="                        { yySkipNL = true; yySeachLParent = false; return TOKEN(TCEQ); }
<normal>"!="                        { yySkipNL = true; yySeachLParent = false; return TOKEN(TCNE); }
<normal>"<"                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TCLT); }
<normal>"<="                        { yySkipNL = true; yySeachLParent = false; return TOKEN(TCLE); }
<normal>">"                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TCGT); }
<normal>">="                        { yySkipNL = true; yySeachLParent = false; return TOKEN(TCGE); }
<normal>"("                         {
    yySkipNL = true;
    bool searchLParent = yySeachLParent;
    yySeachLParent = false;
    if(searchLParent){
        yyReturn(TOK_NO_SPACE, TLPAREN);
    }
    else{
        return TOKEN(TLPAREN);
    }
}
<normal>")"                         { yySkipNL = false; yySeachLParent = false; return TOKEN(TRPAREN); }
<normal>"{"                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TLBRACE); }
<normal>"}"                         {
    yySeachLParent = false;
    if(!yySkipNL){
        yyReturn(TOK_IMPLICIT_END_INSTR, TRBRACE);
    }
    else{
        yySkipNL = false;
        return TOKEN(TRBRACE);
    }
}
<normal>"."                         { yySkipNL = false; yySeachLParent = false; return TOKEN(TDOT); }
<normal>","                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TCOMMA); }
<normal>"+"                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TPLUS); }
<normal>"-"                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TMINUS); }
<normal>"*"                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TMUL); }
<normal>"/"                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TDIV); }
<normal>";"                         { yySkipNL = true; yySeachLParent = false; return TOKEN(TSEMICOL); }
<normal><<EOF>>                     {
    yySeachLParent = false;
    if(!yySkipNL){
        yySkipNL = true;
        yyReturn(TOK_IMPLICIT_END_INSTR, EOF);
    }
    else{
        yyterminate();
    }
}
<normal>.                           printf("Unknown token!\n"); yyterminate();



 /* The following are the rules that keep track of indentation. */
<indent>[ \t]*\n                    {
    yySeachLParent = false;
    g_indent_count = 0;
    g_indent_line_emitted = false; /* ignoring blank line */
}
<indent>" "+                        {
    yySeachLParent = false;
    if(g_indent_context.style == ContextIndentInfo::UNDEFINED){
        g_indent_context.style = ContextIndentInfo::SPACE;
        g_indent_context.spaceSize = yyleng;
    }
#if !INDENT_ALLOW_MIXED
    else if(g_indent_context.style == ContextIndentInfo::TAB){
#if INDENT_FAIL_ON_ERROR
        lyyerror("indentation defined as tab indent, spaces found : ");
#else
        return TOK_BAD_INDENT;
#endif
    }
#endif
    if(yyleng % g_indent_context.spaceSize != 0){
#if INDENT_FAIL_ON_ERROR
        lyyerror("bad space count : ");
#else
        return TOK_BAD_INDENT;
#endif
    }
    g_indent_count += yyleng / g_indent_context.spaceSize;
}
<indent>\t      {
    yySeachLParent = false;
    if(g_indent_context.style == ContextIndentInfo::UNDEFINED){
            g_indent_context.style = ContextIndentInfo::TAB;
    }
#if !INDENT_ALLOW_MIXED
    else if(g_indent_context.style == ContextIndentInfo::SPACE) {
#if INDENT_FAIL_ON_ERROR
        lyyerror("indentation defined as space indent, tab found : ");
#else
        return TOK_BAD_INDENT;
#endif
    }
#endif
    ++g_indent_count;
}
<indent><<EOF>>                             {
    yySeachLParent = false;
    // When encountering the end of file, we want to emit an
    // outdent for all indents currently left.
    for(int i = 0; i < g_indent_context.indentCount; i++){
        yyTokenQueue.push(TOK_OUTDENT);
        yyTokenQueue.push(TOK_IMPLICIT_END_INSTR);
    }
    yyTokenQueue.push(EOF);
}

<indent>.                                   {
    yySeachLParent = false;
    set_yycolumn(yycolumn-1);
    if(g_indent_count == g_indent_context.indentCount) {
        yyless(0);
        BEGIN(indent_caller);
    }
    else if(g_indent_count < g_indent_context.indentCount){
        --g_indent_context.indentCount;
        yyless(0);
        yyReturn(TOK_OUTDENT, TOK_IMPLICIT_END_INSTR);
    }
    else if(g_indent_count > g_indent_context.indentCount){
        yyless(0);
        if(g_indent_line_emitted) {
            g_indent_count = g_indent_context.indentCount;
#if INDENT_FAIL_ON_ERROR
            lyyerror("indentation increased more than once");
#else
            return TOK_BAD_INDENT;
#endif
        }
        else{
            ++g_indent_context.indentCount;
            g_indent_line_emitted = true;
            return TOK_INDENT;
        }
    }
}

%%
