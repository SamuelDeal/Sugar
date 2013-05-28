%{
#include <string>
#include <iostream>
#include <string.h>
#include <queue>
#include <stdarg.h>
#include <stack>
#include "config.h"

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
#ifndef INDENT_ALLOW_MIXED
#define INDENT_ALLOW_MIXED 0
#endif
#ifndef INDENT_FAIL_ON_ERROR
#define INDENT_FAIL_ON_ERROR 0
#endif
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

extern const char* g_current_filename;
bool yySkipNL = true;
std::stack<bool> yyIndentSensitiveStack;
std::queue<int> yyTokenQueue;
std::queue<std::string *> yyIdQueue;
bool yySeachLParent = false;

void yyQueueTokens(int count, ...){
    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++){
        yyTokenQueue.push(va_arg(ap, int));
        yyIdQueue.push(NULL);
    }
    va_end(ap);
}

void yyQueueToken(int token){
    yyTokenQueue.push(token);
    yyIdQueue.push(NULL);
}

void yyQueueTokenId(int token, std::string *id) {
    yyTokenQueue.push(token);
    yyIdQueue.push(id);
}

void yyIndentCheck();

/* Don't mangle yylex please! */
#define YY_DECL extern "C" int yylex()

#if defined(DEBUG_LEXER) && DEBUG_LEXER
#define LOG_LEXER(str, ...) fprintf(stderr, str "\n", ##__VA_ARGS__)
#else
#define LOG_LEXER(str, ...)
#endif

#define YY_USER_INIT { \
        g_indent_context.spaceSize = -1; \
        g_indent_context.indentCount = 0; \
        g_indent_context.style = ContextIndentInfo::UNDEFINED; \
        /*yylloc.first_line = 1;*/ \
        /*yylloc.first_column = 1;*/ \
        LOG_LEXER("Entering Indent state"); \
        BEGIN(yy_state_indent); \
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

#include "ast/all.h"
#include "parser.hpp"
//#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
//#define TOKEN(t) (yylval.token = t)
//extern "C" int yywrap() { }

extern FILE * yyin;

#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(X,##__VA_ARGS__, 4, 3, 2, 1, 0)

#define yyPopQueue() \
    if(!yyTokenQueue.empty()){ \
        int result = yyTokenQueue.front(); \
        std::string *id = yyIdQueue.front(); \
        yyTokenQueue.pop(); \
        yyIdQueue.pop(); \
        if(EOF == result){ \
            yyterminate(); \
        } \
        else if(id == NULL){ \
            yylval.token = result; \
            return result; \
        } \
        else{ \
            yylval.string = id;\
            return result; \
        } \
    }

#define yyReturn(...) \
        yyQueueTokens(VA_NARGS(__VA_ARGS__), ##__VA_ARGS__); \
        yyPopQueue();

#define yyReturnId(t) \
        yyQueueTokenId(t, new std::string(yytext, yyleng)); \
        yyPopQueue();


#if INTERACTIVE_INPUT && ((defined(LIBREADLINE) && LIBREADLINE) || (defined(LIBEDIT) && LIBEDIT))

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    lexer_input((char *)buf, &result, max_size)

static void lexer_input(char *buf, int *result, int max);

#endif
%}



 /* This is a sub-parser (state) for indentation-sensitive scoping */
%s yy_state_indent

 /* %option 8bit reentrant bison-bridge */
%option warn
%option nodefault
%option yylineno
%option noyywrap

 /*%option debug*/
%%
    yyReturn();

<yy_state_indent>[ \t]*\r\n|[ \t]*\r|[ \t]*\n   { /* ignoring blank line */
    yySeachLParent = false;
    g_indent_count = 0;
}

<yy_state_indent>" "+            {
    LOG_LEXER("spaces found!!!");
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

<yy_state_indent>\t+             {
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
    g_indent_count += yyleng;
}

\r\n|\r|\n                  {
    yySeachLParent = false;
    g_indent_count = 0;
    BEGIN(yy_state_indent);
    LOG_LEXER("Entering Indent state");
    if(!yySkipNL){
        yySkipNL = true;
        yyReturn(TOK_NL);
    }
    yyReturn();
}
[ \t]                       { yySeachLParent = false; };
"return"                    { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturn(TRETURN); }
"if"                        { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturn(TIF); }
"else"                      { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturn(TELSE); }
"true"                      { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturn(TTRUE); }
"false"                     { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturn(TFALSE); }
[A-Z][a-zA-Z0-9_\?]*        { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturnId(TTYPENAME); }
[a-z_][a-zA-Z0-9_\?!]*      { yySkipNL = false; yySeachLParent = true; yyIndentCheck(); yyReturnId(TIDENTIFIER); }
[0-9]+\.[0-9]*              { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturnId(TDOUBLE); }
[0-9]+                      { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturnId(TINTEGER); }
"="                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TEQUAL); }
"=="                        { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturnId(TCEQ); }
"!="                        { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TCNE); }
"<"                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TCLT); }
"<="                        { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TCLE); }
">"                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TCGT); }
">="                        { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TCGE); }
"."                         { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturn(TDOT); }
","                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TCOMMA); }
"+"                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturnId(TPLUS); }
"-"                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturnId(TMINUS); }
"*"                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturnId(TMUL); }
"/"                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturnId(TDIV); }
";"                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TSEMICOL); }
"("                         {
    yySkipNL = true;
    bool searchLParent = yySeachLParent;
    yySeachLParent = false;
    yyIndentCheck();
    if(searchLParent){
        yyReturn(TOK_NO_SPACE, TLPAREN);
    }
    else{
        yyReturn(TLPAREN);
    }
}
")"                         { yySkipNL = false; yySeachLParent = false; yyIndentCheck(); yyReturn(TRPAREN); }
"{"                         { yySkipNL = true; yySeachLParent = false; yyIndentCheck(); yyReturn(TLBRACE); }
"}"                         {
    yySeachLParent = false;
    yyIndentCheck();
    if(!yySkipNL){
        yyReturn(TOK_IMPLICIT_END_INSTR, TRBRACE);
    }
    else{
        yySkipNL = false;
        yyReturn(TRBRACE);
    }
}

<*><<EOF>>                  {
    if(!yySkipNL){
        yyQueueToken(TOK_IMPLICIT_END_INSTR);
    }
    while(g_indent_context.indentCount > 0){
        --g_indent_context.indentCount;
        yyQueueToken(TOK_OUTDENT);
        yyQueueToken(TOK_IMPLICIT_END_INSTR);
    }
    yyReturn(EOF);
}
.                           {   printf("Unknown token!\n"); yyterminate();  }

%%

void yyIndentCheck(){
    if(YY_START != yy_state_indent){
        return;
    }
    LOG_LEXER("Leaving Indent state");
    BEGIN(INITIAL);
    if(yyIndentSensitiveStack.empty() || !yyIndentSensitiveStack.top()){
        return;
    }
    while(g_indent_count < g_indent_context.indentCount){
        --g_indent_context.indentCount;
        yyQueueToken(TOK_OUTDENT);
        yyQueueToken(TOK_IMPLICIT_END_INSTR);
    }
    if(g_indent_count > g_indent_context.indentCount){
        yyQueueToken(TOK_INDENT);
        ++g_indent_context.indentCount;
#if INDENT_FAIL_ON_ERROR
        if(g_indent_count > g_indent_context.indentCount){
            lyyerror("indentation increased more than once");
        }
#else
        while(g_indent_count > g_indent_context.indentCount){
            ++g_indent_context.indentCount;
            yyQueueToken(TOK_BAD_INDENT);
        }
#endif
    }

}
