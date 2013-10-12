%{
#include <string>
#include <iostream>
#include <string.h>
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

#include "lexer_ctx.h"

LexerContext lexerCtx;



/* Don't mangle yylex please! */
#define YY_DECL extern "C" int yylex()

#if defined(DEBUG_LEXER) && DEBUG_LEXER
#define LOG_LEXER(str, ...) fprintf(stderr, "\n" str "\n", ##__VA_ARGS__)
#else
#define LOG_LEXER(str, ...)
#endif


#define lexPushState(state) { \
    int old = YY_START; \
    LOG_LEXER(" adding state: %d", __LINE__); \
    yy_push_state(state); \
    LOG_LEXER(" ++ state added: %d (from %d)", state, old); \
}


#define lexPopState() { \
    int old = YY_START; \
    LOG_LEXER(" poping state: %d", __LINE__); \
    yy_pop_state(); \
    LOG_LEXER(" -- state poped: %d, (current: %d)", old, YY_START); \
}

#define YY_USER_INIT { \
        lexerCtx.indentContext.spaceSize = -1; \
        lexerCtx.indentContext.style = IndentInfo::UNDEFINED; \
        lexerCtx.lineIndents = 0; \
        lexerCtx.indents.push(0); \
        lexerCtx.eof = false; \
        LOG_LEXER("Entering Indent state"); \
        lexPushState(lex_state_newline); \
    }

void lyyerror(const char *s){
    //fprintf(stderr, "%s: %d:%d: %s: %s\n", g_current_filename, yylineno, yycolumn, s, yytext);
    //fprintf(stderr, "%d:%d: %s: %s\n", yylineno, yycolumn, s, yytext);
    fprintf(stderr, "%s: %s\n", s, yytext);
    exit(1);
}

/*
extern "C" {

int yywrap() {
    if(lexerCtx.eof && lexerCtx.tokens.empty()) {
        fclose(yyin);
        return 0;
    }
    else{
        return 1;
    }
}

}*/

#include "ast/all.h"
#include "parser.hpp"
//#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
//#define TOKEN(t) (yylval.token = t)
//extern "C" int yywrap() { }

extern FILE * yyin;

#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(X,##__VA_ARGS__, 4, 3, 2, 1, 0)

#define yyReturnId(foundToken) \
    yylval.string = new std::string(strdup(yytext), yyleng); \
    return foundToken;

#if INTERACTIVE_INPUT && ((defined(LIBREADLINE) && LIBREADLINE) || (defined(LIBEDIT) && LIBEDIT))

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    lexer_input((char *)buf, &result, max_size)

static void lexer_input(char *buf, int *result, int max);

#endif


%}



 /* This is a sub-parser (state) for indentation-sensitive scoping */
%s lex_state_indent
%s lex_state_newline
%s lex_state_skipindent
%s lex_state_skipnl
%s lex_state_spaceaware
%s lex_state_unstack
%s lex_state_blockbegin
%s lex_state_blockend

 /* %option 8bit reentrant bison-bridge */
%option warn
/*%option nodefault*/
%option noyywrap
%option stack

 /*%option debug*/
%%
    if(lexerCtx.eof){
        if(lexerCtx.tokens.empty()) {
            LOG_LEXER("End of file !!!!!");
            yyterminate();
        }
        else{
            int result = lexerCtx.tokens.front();
            lexerCtx.tokens.pop();
            return result;
        }
    }

<*><<EOF>>                  {
    if(lexerCtx.eof){
        int result = lexerCtx.tokens.front();
        lexerCtx.tokens.pop();
        return result;
    }
    else{
        lexerCtx.eof = true;
        lexerCtx.tokens.push(TOK_END_INSTR);
        while(lexerCtx.indents.top() > 0){
            --lexerCtx.indents.top();
            lexerCtx.tokens.push(TOK_OUTDENT);
        }
    }
}

<lex_state_blockbegin>{
    [ \t]*\r\n|[ \t]*\r|[ \t]*\n   { /* ignoring blank line */
    }

    [ \t]*  {
        yyless(0);
        lexerCtx.lineIndents = 0;
        lexerCtx.indents.push(0);
        lexPushState(lex_state_indent);
    }

    "{"                          {
        lexerCtx.indents.pop();
        while(!lexerCtx.latentTokens.empty()){
            lexerCtx.latentTokens.pop();
        }
        lexPopState();
        lexerCtx.indents.push(0);
        return TLBRACE;
    }

    .                               {
        yyless(0);
        lexerCtx.indents.pop();
        bool indentFound = false;
        while(!lexerCtx.latentTokens.empty()){
            if(!indentFound && lexerCtx.latentTokens.front() == TOK_INDENT){
                lexerCtx.tokens.push(TOK_INDENT);
                indentFound = true;
            }
            lexerCtx.latentTokens.pop();
        }
        lexPopState();
        if(!indentFound){
            lexerCtx.tokens.push(TOK_BAD_INDENT);
        }
        else{
            lexerCtx.indents.push(0);
        }
        lexPushState(lex_state_unstack);
    }

}

<lex_state_blockend>{
    [ \t]*\r\n|[ \t]*\r|[ \t]*\n   { /* ignoring blank line */
    }

    [ \t]*  {
        yyless(0);
        lexerCtx.lineIndents = 0;
        lexPushState(lex_state_indent);
    }

    .                               {
        yyless(0);
        while(!lexerCtx.latentTokens.empty()){
            lexerCtx.tokens.push(lexerCtx.latentTokens.front());
            lexerCtx.latentTokens.pop();
        }
        lexPopState();
        if(!lexerCtx.tokens.empty()){
            lexPushState(lex_state_unstack);
        }
    }

}

<lex_state_newline>{
    [ \t]*\r\n|[ \t]*\r|[ \t]*\n   { /* ignoring blank line */
    }

    [ \t]*  {
        yyless(0);
        lexerCtx.lineIndents = 0;
        lexPushState(lex_state_indent);
    }

    "else"                          {
        yyless(0);
        lexPopState();

    }

    .                               {
        yyless(0);
        while(!lexerCtx.latentTokens.empty()){
            lexerCtx.tokens.push(lexerCtx.latentTokens.front());
            lexerCtx.latentTokens.pop();
        }
        lexPopState();
        if(!lexerCtx.tokens.empty()){
            lexPushState(lex_state_unstack);
        }
    }

}

<lex_state_indent>{
    " "+            {
        LOG_LEXER("HERE 1 !!! %d", lexerCtx.indentContext.style);
        if(lexerCtx.indentContext.style == IndentInfo::UNDEFINED){
            LOG_LEXER("HERE 1.1 !!! %d", lexerCtx.indentContext.style);
            lexerCtx.indentContext.style = IndentInfo::SPACE;
            lexerCtx.indentContext.spaceSize = yyleng;
        }
#if !INDENT_ALLOW_MIXED
        else if(lexerCtx.indentContext.style == IndentInfo::TAB){
            LOG_LEXER("HERE 1.2 !!! %d", lexerCtx.indentContext.style);
            return TOK_BAD_INDENT;
        }
#endif
        if(yyleng % lexerCtx.indentContext.spaceSize != 0 || yyleng == 1){
            return TOK_BAD_INDENT;
        }
        lexerCtx.lineIndents += yyleng / lexerCtx.indentContext.spaceSize;
    }

    \t+             {
        LOG_LEXER("HERE 2 !!!");
        if(lexerCtx.indentContext.style == IndentInfo::UNDEFINED){
            lexerCtx.indentContext.style = IndentInfo::TAB;
        }
#if !INDENT_ALLOW_MIXED
        else if(lexerCtx.indentContext.style == IndentInfo::SPACE) {
            return TOK_BAD_INDENT;
        }
#endif
        lexerCtx.lineIndents += yyleng;
    }

    .               {
        yyless(0);
        //if(lexerCtx.indentsSensitive.top()){
            while(lexerCtx.lineIndents < lexerCtx.indents.top()){
                --lexerCtx.indents.top();
                lexerCtx.latentTokens.push(TOK_OUTDENT);
            }
            if(lexerCtx.lineIndents > lexerCtx.indents.top()){
                lexerCtx.latentTokens.push(TOK_INDENT);
                ++lexerCtx.indents.top();
                while(lexerCtx.lineIndents > lexerCtx.indents.top()){
                    ++lexerCtx.indents.top();
                    lexerCtx.latentTokens.push(TOK_BAD_INDENT);
                }
            }
        //}
        lexPopState();
    }
}

<lex_state_skipindent>{
    [ \t]*\r\n|[ \t]*\r|[ \t]*\n   { /* ignoring blank line */
    }

    [ \t]*                          {
        lexPopState();
    }

    .                               {
        yyless(0);
        lexPopState();
    }
}

<lex_state_skipnl>{
    [ \t]*\r\n|[ \t]*\r|[ \t]*\n   { /* ignoring blank line */
    }

    \r\n|\r|\n*                     {
        lexPopState();
    }

    .                               {
        yyless(0);
        lexPopState();
    }
}

<lex_state_spaceaware>{
    ([ \r\n\t]+)+                   {
        lexPopState();
    }

    "("                             {
        lexerCtx.tokens.push(TOK_NO_SPACE);
        lexerCtx.tokens.push(TLPAREN);
        lexPopState();
        lexPushState(lex_state_unstack);
    }

    .                               {
        yyless(0);
        lexPopState();
    }
}

<lex_state_unstack>{
    .                   {
        yyless(0);
        if(lexerCtx.tokens.empty()){
            lexPopState();
        }
        else{
            int result = lexerCtx.tokens.front();
            lexerCtx.tokens.pop();
            return result;
        }
    }
}

<INITIAL>{
    [\t ]*                      {}
    \r\n|\r|\n                  {
        lexerCtx.latentTokens.push(TOK_END_INSTR);
        lexPushState(lex_state_newline);
    }
}

<INITIAL>{
    "return"                    { return TRETURN; }
    "if"                        { return TIF; }
    "else"                      { return TELSE; }
    "true"                      { return TTRUE; }
    "false"                     { return TFALSE; }
    [A-Z][a-zA-Z0-9_\?]*        { yyReturnId(TTYPENAME); }
    [a-z_][a-zA-Z0-9_\?!]*      { lexPushState(lex_state_spaceaware); yyReturnId(TIDENTIFIER); }
    [0-9]+\.[0-9]*              { yyReturnId(TDOUBLE); }
    [0-9]+                      { yyReturnId(TINTEGER); }
    "="                         { return TEQUAL; }
    "=="                        { yyReturnId(TCEQ); }
    "!="                        { return TCNE; }
    "<"                         { return TCLT; }
    "<="                        { return TCLE; }
    ">"                         { return TCGT; }
    ">="                        { return TCGE; }
    "."                         { return TDOT; }
    ","                         { return TCOMMA; }
    "+"                         { lexPushState(lex_state_skipnl); yyReturnId(TPLUS); }
    "-"                         { lexPushState(lex_state_skipnl); yyReturnId(TMINUS); }
    "*"                         { lexPushState(lex_state_skipnl); yyReturnId(TMUL); }
    "/"                         { lexPushState(lex_state_skipnl); yyReturnId(TDIV); }
    ";"                         { return TOK_END_INSTR; }
    "("                         {
        //lexerCtx.indents.push(false);
        //lexPushState(lex_state_skipnl);
        return TLPAREN;
    }
    ")"                         {
        //lexerCtx.indentSensitive.pop();
        //lexPopState();
        return TRPAREN;
    }
    "{"                         {
        while(!lexerCtx.latentTokens.empty()){
            lexerCtx.latentTokens.pop();
        }
        //lexerCtx.indentSensitive.push(false);
        lexerCtx.indents.push(0);
        return TLBRACE;
    }
    "}"                         {
        lexerCtx.tokens.push(TOK_END_INSTR);
        lexerCtx.tokens.push(TRBRACE);
        lexerCtx.latentTokens.push(TOK_END_INSTR);
        lexPushState(lex_state_blockend);
        lexPushState(lex_state_unstack);
        lexerCtx.indents.pop();
    }
    .                           {   LOG_LEXER("Unknown token!!!!!"); yyterminate();  }
}



%%

void lexStartBlock() {
    lexPushState(lex_state_blockbegin);
}
