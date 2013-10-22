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


const char * const  state_names[] = {
"INITIAL",
"normal",
"skipline",
"mlcomment",
"newline",
"skipnl",
"spaceaware",
"unstack",
"rbrace",
"rbrace_newline"
};

#define lexPushState(state) { \
    int old = YY_START; \
    yy_push_state(state); \
    LOG_LEXER(" ++ state added: %s (from %s, line %d)", state_names[state], state_names[old], __LINE__); \
}


#define lexPopState() { \
    int old = YY_START; \
    yy_pop_state(); \
    LOG_LEXER(" -- state poped: %s, (current: %s, line %d)", state_names[old], state_names[YY_START], __LINE__); \
}

#define YY_USER_INIT { \
        lexerCtx.indentContext.spaceSize = -1; \
        lexerCtx.indentContext.style = IndentInfo::UNDEFINED; \
        lexerCtx.lineIndents = 0; \
        lexerCtx.indents.push(new IndentContext()); \
        lexerCtx.indents.top()->start = 0; \
        lexerCtx.indents.top()->current = 0; \
        lexerCtx.eof = false; \
        lexerCtx.pendingEndInstr = false; \
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
%s lex_state_normal
%s lex_state_skipline
%s lex_state_mlcomment
%s lex_state_newline
%s lex_state_skipnl
%s lex_state_spaceaware
%s lex_state_unstack
%s lex_state_rbrace
%s lex_state_rbrace_newline

 /* %option 8bit reentrant bison-bridge */
%option warn
%option nodefault
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
        LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
        lexerCtx.tokens.push(TOK_END_INSTR);
        while(lexerCtx.indents.top()->current > lexerCtx.indents.top()->start){
            --lexerCtx.indents.top()->current;
            lexerCtx.tokens.push(TOK_OUTDENT);
            LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
            lexerCtx.tokens.push(TOK_END_INSTR);
        }
    }
}

<INITIAL>{
    "#!" {
        lexPushState(lex_state_skipline);
    }
    \r\n|\r|\n {
        lexPushState(lex_state_normal);
    }

    . {
        yyless(0);
        lexPushState(lex_state_normal);
    }
}

<lex_state_skipline>{
    \r\n|\r|\n {
        yyless(0);
        lexPopState();
    }

    . { // do nothing
    }
}

<lex_state_mlcomment>{
    \*+\/          {
        if(lexerCtx.commentStartCount < yyleng){
            lexPopState();
        }
    }

    \r\n|\r|\n  {
    }

    . {
    }
}

<lex_state_newline>{
    "else"                          {
        yyless(0);
        if(lexerCtx.indents.top()->start == -1){
            lexerCtx.pendingEndInstr = false;
            lexerCtx.indents.top()->start = lexerCtx.lineIndents;
            lexerCtx.indents.top()->current = lexerCtx.lineIndents;
        }
        else if(lexerCtx.lineIndents == lexerCtx.indents.top()->current){
            if(lexerCtx.pendingEndInstr){
                lexerCtx.pendingEndInstr = false;
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                lexerCtx.tokens.push(TOK_END_INSTR);
            }
        }
        else if(lexerCtx.lineIndents > lexerCtx.indents.top()->current){
            lexerCtx.pendingEndInstr = false;
            lexerCtx.tokens.push(TOK_INDENT);
            ++lexerCtx.indents.top()->current;
            while(lexerCtx.lineIndents > lexerCtx.indents.top()->current){
                ++lexerCtx.indents.top()->current;
                lexerCtx.tokens.push(TOK_BAD_INDENT);
            }
        }
        else{
            lexerCtx.pendingEndInstr = false;
            while(lexerCtx.lineIndents < lexerCtx.indents.top()->current){
                --lexerCtx.indents.top()->current;
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                lexerCtx.tokens.push(TOK_END_INSTR);
                lexerCtx.tokens.push(TOK_OUTDENT);
            }
        }
        lexPopState();
        if(!lexerCtx.tokens.empty()){
            lexPushState(lex_state_unstack);
        }
    }

    "." {
        while(!lexerCtx.latentTokens.empty()){
            lexerCtx.latentTokens.pop();
        }
        yyless(0);
        lexPopState();
    }

    "{" {
        while(!lexerCtx.latentTokens.empty()){
            lexerCtx.latentTokens.pop();
        }
        yyless(0);
        lexPopState();
    }





}

<lex_state_rbrace>{
    [ \t]* { /* ignoring spaces */
    }

    \r\n|\r|\n {
        yyless(0);
        lexPopState();
        lexerCtx.lineIndents = 0;
        lexPushState(lex_state_rbrace_newline);
    }

    "else" {
        yyless(0);
        lexerCtx.pendingEndInstr = false;
        lexPopState();
        lexPushState(lex_state_newline);
    }

    . {
        yyless(0);
        lexerCtx.pendingEndInstr = true;
        lexPopState();
        lexPushState(lex_state_newline);
    }
}

<lex_state_rbrace_newline>{
    "else" {
        yyless(0);
        lexerCtx.pendingEndInstr = false;
        lexPopState();
        lexPushState(lex_state_newline);
    }
}

<lex_state_newline,lex_state_rbrace_newline>{
    [ \t]*\r\n|[ \t]*\r|[ \t]*\n   { /* ignoring blank line */
        lexerCtx.lineIndents = 0;
    }

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

    "}" {
        while(!lexerCtx.latentTokens.empty()){
            lexerCtx.latentTokens.pop();
        }
        yyless(0);
        lexPopState();
    }

    .                               {
        yyless(0);
        if(lexerCtx.indents.top()->start == -1){
            lexerCtx.pendingEndInstr = false;
            lexerCtx.indents.top()->start = lexerCtx.lineIndents;
            lexerCtx.indents.top()->current = lexerCtx.lineIndents;
        }
        else if(lexerCtx.lineIndents == lexerCtx.indents.top()->current){
            if(lexerCtx.pendingEndInstr){
                lexerCtx.pendingEndInstr = false;
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                lexerCtx.tokens.push(TOK_END_INSTR);
            }
        }
        else if(lexerCtx.lineIndents > lexerCtx.indents.top()->current){
            lexerCtx.pendingEndInstr = false;
            lexerCtx.tokens.push(TOK_INDENT);
            ++lexerCtx.indents.top()->current;
            while(lexerCtx.lineIndents > lexerCtx.indents.top()->current){
                ++lexerCtx.indents.top()->current;
                lexerCtx.tokens.push(TOK_BAD_INDENT);
            }
        }
        else{
            lexerCtx.pendingEndInstr = false;
            while(lexerCtx.lineIndents < lexerCtx.indents.top()->current){
                --lexerCtx.indents.top()->current;
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                lexerCtx.tokens.push(TOK_END_INSTR);
                lexerCtx.tokens.push(TOK_OUTDENT);
            }
            LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
            lexerCtx.tokens.push(TOK_END_INSTR);
        }
        lexPopState();
        if(!lexerCtx.tokens.empty()){
            lexPushState(lex_state_unstack);
        }
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
    [ \t]*\r\n|[ \t]*\r|[ \t]*\n   { /* ignoring blank line */
        yyless(0);
        lexerCtx.pendingEndInstr = true;
        lexerCtx.lineIndents = 0;
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
    \r\n|\r|\n          {
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



<lex_state_normal>{
    [\t ]*                      {}
    \r\n|\r|\n                  {
        lexerCtx.pendingEndInstr = true;
        lexerCtx.lineIndents = 0;
        lexPushState(lex_state_newline);
    }
    "//"                        { lexPushState(lex_state_skipline); }
    \#[ \t]                     { lexPushState(lex_state_skipline); }
    \/\*+                       {
        lexPushState(lex_state_mlcomment);
        lexerCtx.commentStartCount = yyleng -1;
    }
    "return"                    { return TRETURN; }
    "if"                        { return TIF; }
    "else"                      { return TELSE; }
    "true"                      { return TTRUE; }
    "false"                     { return TFALSE; }
    "and"                       { return TAND; }
    "&&"                        { return TAND; }
    "or"                        { return TOR;  }
    "||"                        { return TOR;  }
    [A-Z][a-zA-Z0-9_\?]*        { yyReturnId(TTYPENAME); }
    [a-z_][a-zA-Z0-9_\?!]*      { lexPushState(lex_state_spaceaware); yyReturnId(TIDENTIFIER); }
    [0-9]+\.[0-9]*              { yyReturnId(TDOUBLE); }
    [0-9]+                      { yyReturnId(TINTEGER); }
    "="                         { return TEQUAL; }
    "=="                        { yyReturnId(TCEQ); }
    "!="                        { yyReturnId(TCNE); }
    "<"                         { yyReturnId(TCLT); }
    "<="                        { yyReturnId(TCLE); }
    ">"                         { yyReturnId(TCGT); }
    ">="                        { yyReturnId(TCGE); }
    "."                         { return TDOT; }
    ","                         { lexPushState(lex_state_skipnl); return TCOMMA; }
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
        lexerCtx.indents.push(new IndentContext());
        lexerCtx.indents.top()->start = -1;
        lexerCtx.indents.top()->current = -1;
        return TLBRACE;
    }
    "}"                         {
        LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
        lexerCtx.tokens.push(TOK_END_INSTR);
        if(lexerCtx.indents.top()->start != -1){
            while(lexerCtx.indents.top()->current > lexerCtx.indents.top()->start){
                --lexerCtx.indents.top()->current;
                lexerCtx.tokens.push(TOK_OUTDENT);
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                lexerCtx.tokens.push(TOK_END_INSTR);
            }
        }

        lexerCtx.tokens.push(TRBRACE);
        lexPushState(lex_state_rbrace);
        lexPushState(lex_state_unstack);
        delete lexerCtx.indents.top();
        lexerCtx.indents.pop();
    }
    .                           {
        LOG_LEXER("Unknown token!!!!!");
        yyterminate();
    }
}

%%
