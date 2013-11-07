%top{

#include "../config_checked.h"
#include "LexerContext.h"

#define YY_EXTRA_TYPE sugar::parser::LexerContext*
#include "parser.hpp"

}

%{

#include <string>
#include <iostream>
#include <string.h>

#if DEBUG_LEXER
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

#define lexPushState(state, yyscanner) { \
    int old = YY_START; \
    yy_push_state(state, yyscanner); \
    LOG_LEXER(" ++ state added: %s (from %s, line %d)", state_names[state], state_names[old], __LINE__); \
}

#define lexPopState(yyscanner) { \
    int old = YY_START; \
    yy_pop_state(yyscanner); \
    LOG_LEXER(" -- state poped: %s, (current: %s, line %d)", state_names[old], state_names[YY_START], __LINE__); \
}

#define YY_USER_ACTION \
    yylloc_param->first_line = yylloc_param->last_line = yylineno;

void yyerror(YYLTYPE *locp, sugar::parser::LexerContext *lexerCtx, const char *err){
#if SHELL_USE_COLOR
    if(isatty(fileno(stdout)) == 1){
        std::cout << "\x1b[1m\x1b[37m"  << (lexerCtx->filename == "" ? "stdin" : lexerCtx->filename) << ":" << locp->first_line;
        std::cout << ":" << locp->first_column << ": \x1b[1m\x1b[31m" << "error: \x1b[1m\x1b[37m" << err << "\x1b[0m" << std::endl;
        std::cout << lexerCtx->currentLine << std::endl;
        for(unsigned int i = 0; i < locp->first_column; i++) {
            std::cout << (lexerCtx->currentLine[i] == '\t' ? '\t' : ' ');
        }
        std::cout << "\x1b[1m\x1b[32m^\x1b[0m" << std::endl << std::endl;
    }
    else{
#endif
        std::cout << (lexerCtx->filename == "" ? "stdin" : lexerCtx->filename) << ":" << locp->first_line;
        std::cout << ":" << locp->first_column << ": error: " << err << std::endl;
        std::cout << lexerCtx->currentLine << std::endl;
        for(unsigned int i = 0; i < locp->first_column; i++) {
            std::cout << (lexerCtx->currentLine[i] == '\t' ? '\t' : ' ');
        }
        std::cout << "^" << std::endl << std::endl;
#if SHELL_USE_COLOR
    }
#endif
}

#include "../ast/all.h"

#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(X,##__VA_ARGS__, 4, 3, 2, 1, 0)

#define yyReturnId(foundToken, yyscanner) \
    yyget_lval(yyscanner)->string = new std::string(strdup(yytext), yyleng); \
    return foundToken;

#if INTERACTIVE_INPUT && (LIBREADLINE || LIBEDIT)

#undef  YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    lexer_input((char *)buf, &result, max_size, yyscanner)

static void lexer_input(char *buf, int *result, int max, void* yyscanner);

#endif

%}



/* flex states */
%s lex_state_normal
%s lex_state_skipline
%s lex_state_mlcomment
%s lex_state_newline
%s lex_state_skipnl
%s lex_state_spaceaware
%s lex_state_unstack
%s lex_state_rbrace
%s lex_state_rbrace_newline

/* flex options */
%option debug
%option warn
%option nodefault
%option noyywrap
%option stack
%option reentrant
%option bison-bridge
%option noyylineno
%option bison-locations


%%
    yylloc_param->first_column += yyleng;
    LOG_LEXER("column: %d (+%d)", yylloc_param->first_column, yyleng);
    if(yyget_extra(yyscanner)->eof){
        if(yyget_extra(yyscanner)->tokens.empty()) {
            LOG_LEXER("End of file !!!!!");
            yyterminate();
        }
        else{
            int result = yyget_extra(yyscanner)->tokens.front();
            yyget_extra(yyscanner)->tokens.pop();
            return result;
        }
    }
<*><<EOF>>                  {
    if(yyget_extra(yyscanner)->eof){
        int result = yyget_extra(yyscanner)->tokens.front();
        yyget_extra(yyscanner)->tokens.pop();
        return result;
    }
    else{
        yyget_extra(yyscanner)->eof = true;
        LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
        yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
        while(yyget_extra(yyscanner)->indents.top()->current > yyget_extra(yyscanner)->indents.top()->start){
            --yyget_extra(yyscanner)->indents.top()->current;
            yyget_extra(yyscanner)->tokens.push(TOK_OUTDENT);
            LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
            yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
        }
    }
}

<INITIAL>{
    #![^\r\n]* {
        lexPushState(lex_state_normal, yyscanner);
    }

    "\r\n" {
        yyless(1);
    }

    [\r\n] {
        yyless(0);
        lexPushState(lex_state_normal, yyscanner);
    }

    [^\r\n]* {
        yylloc_param->first_column = 0;
        yyget_extra(yyscanner)->currentLine = "";
        yyget_extra(yyscanner)->currentLine.append(yytext, yyleng);
        LOG_LEXER("copying line: %s", yyget_extra(yyscanner)->currentLine.c_str());
        yyless(0);
        lexPushState(lex_state_normal, yyscanner);
    }
}

<lex_state_skipline>{
    "\r\n" {
        yyless(1);
    }

    [\r\n] {
        yyless(0);
        lexPopState(yyscanner);
    }

    . { // do nothing
    }
}

<lex_state_mlcomment>{
    \*+\/          {
        yylloc_param->first_column += yyleng;
        if(yyget_extra(yyscanner)->commentStartCount < yyleng){
            lexPopState(yyscanner);
        }
    }

    "\r\n"  {
        yyless(1);
    }

    [\r\n][^\r\n]* {
        ++yylineno;
        yylloc_param->first_column = 0;
        yyget_extra(yyscanner)->currentLine = "";
        yyget_extra(yyscanner)->currentLine.append(yytext+1, yyleng-1);
        LOG_LEXER("copying line 2: %s", yyget_extra(yyscanner)->currentLine.c_str());
        yyless(1);
    }

    . {
        ++yylloc_param->first_column;
    }
}

<lex_state_newline>{
    "else"                          {
        yyless(0);
        if(yyget_extra(yyscanner)->indents.top()->start == -1){
            yyget_extra(yyscanner)->pendingEndInstr = false;
            yyget_extra(yyscanner)->indents.top()->start = yyget_extra(yyscanner)->lineIndents;
            yyget_extra(yyscanner)->indents.top()->current = yyget_extra(yyscanner)->lineIndents;
        }
        else if(yyget_extra(yyscanner)->lineIndents == yyget_extra(yyscanner)->indents.top()->current){
            if(yyget_extra(yyscanner)->pendingEndInstr){
                yyget_extra(yyscanner)->pendingEndInstr = false;
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
            }
        }
        else if(yyget_extra(yyscanner)->lineIndents > yyget_extra(yyscanner)->indents.top()->current){
            yyget_extra(yyscanner)->pendingEndInstr = false;
            yyget_extra(yyscanner)->tokens.push(TOK_INDENT);
            ++yyget_extra(yyscanner)->indents.top()->current;
            while(yyget_extra(yyscanner)->lineIndents > yyget_extra(yyscanner)->indents.top()->current){
                ++yyget_extra(yyscanner)->indents.top()->current;
                yyget_extra(yyscanner)->tokens.push(TOK_BAD_INDENT);
            }
        }
        else{
            yyget_extra(yyscanner)->pendingEndInstr = false;
            while(yyget_extra(yyscanner)->lineIndents < yyget_extra(yyscanner)->indents.top()->current){
                --yyget_extra(yyscanner)->indents.top()->current;
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
                yyget_extra(yyscanner)->tokens.push(TOK_OUTDENT);
            }
        }
        lexPopState(yyscanner);
        if(!yyget_extra(yyscanner)->tokens.empty()){
            lexPushState(lex_state_unstack, yyscanner);
        }
    }

    "." {
        while(!yyget_extra(yyscanner)->latentTokens.empty()){
            yyget_extra(yyscanner)->latentTokens.pop();
        }
        yyless(0);
        lexPopState(yyscanner);
    }

    "{" {
        while(!yyget_extra(yyscanner)->latentTokens.empty()){
            yyget_extra(yyscanner)->latentTokens.pop();
        }
        yyless(0);
        lexPopState(yyscanner);
    }
}

<lex_state_rbrace>{
    [ \t]* { /* ignoring spaces */
        yylloc_param->first_column += yyleng;
    }

    "\r\n" {
        yyless(1);
    }

    [\r\n] {
        yyless(0);
        lexPopState(yyscanner);
        yyget_extra(yyscanner)->lineIndents = 0;
        lexPushState(lex_state_rbrace_newline, yyscanner);
    }

    "else" {
        yyless(0);
        yyget_extra(yyscanner)->pendingEndInstr = false;
        lexPopState(yyscanner);
        lexPushState(lex_state_newline, yyscanner);
    }

    . {
        yyless(0);
        yyget_extra(yyscanner)->pendingEndInstr = true;
        lexPopState(yyscanner);
        lexPushState(lex_state_newline, yyscanner);
    }
}

<lex_state_rbrace_newline>{
    "else" {
        yyless(0);
        yyget_extra(yyscanner)->pendingEndInstr = false;
        lexPopState(yyscanner);
        lexPushState(lex_state_newline, yyscanner);
    }
}

<lex_state_newline,lex_state_rbrace_newline>{
    [ \t]+(\r\n|\r|\n)   { /* ignoring blank line */
        yyless(yyleng-1);
    }

    [\r\n][^\r\n]*       {
        ++yylineno;
        yyget_extra(yyscanner)->lineIndents = 0;
        yylloc_param->first_column = 0;
        yyget_extra(yyscanner)->currentLine = "";
        yyget_extra(yyscanner)->currentLine.append(yytext+1, yyleng-1);
        LOG_LEXER("copying line 4: %s", yyget_extra(yyscanner)->currentLine.c_str());
        yyless(1);
    }

    " "+            {
        yylloc_param->first_column += yyleng;
        LOG_LEXER("HERE 1 !!! %d", yyget_extra(yyscanner)->indentContext.style);
        if(yyget_extra(yyscanner)->indentContext.style == IndentInfo::UNDEFINED){
            LOG_LEXER("HERE 1.1 !!! %d", yyget_extra(yyscanner)->indentContext.style);
            yyget_extra(yyscanner)->indentContext.style = IndentInfo::SPACE;
#if INDENT_SPACE_SIZE > 0
            yyget_extra(yyscanner)->indentContext.spaceSize = yyleng;
#endif
        }
#if !INDENT_ALLOW_MIXED
        else if(yyget_extra(yyscanner)->indentContext.style == IndentInfo::TAB){
            LOG_LEXER("HERE 1.2 !!! %d", yyget_extra(yyscanner)->indentContext.style);
            return TOK_BAD_INDENT;
        }
#endif
        if(yyleng % yyget_extra(yyscanner)->indentContext.spaceSize != 0 || yyleng == 1){
            return TOK_BAD_INDENT;
        }
        yyget_extra(yyscanner)->lineIndents += yyleng / yyget_extra(yyscanner)->indentContext.spaceSize;
        LOG_LEXER("!!! LINE_INDENT 1: %d / %d => %d", yyleng, yyget_extra(yyscanner)->indentContext.spaceSize, yyget_extra(yyscanner)->lineIndents);
    }

    \t+             {
        yylloc_param->first_column += yyleng;
        LOG_LEXER("HERE 2 !!!");
        if(yyget_extra(yyscanner)->indentContext.style == IndentInfo::UNDEFINED){
            yyget_extra(yyscanner)->indentContext.style = IndentInfo::TAB;
        }
#if !INDENT_ALLOW_MIXED
        else if(yyget_extra(yyscanner)->indentContext.style == IndentInfo::SPACE) {
            return TOK_BAD_INDENT;
        }
#endif
        yyget_extra(yyscanner)->lineIndents += yyleng;
        LOG_LEXER("!!! LINE_INDENT 2: %d, => %d", yyleng, yyget_extra(yyscanner)->lineIndents);
    }

    "}" {
        while(!yyget_extra(yyscanner)->latentTokens.empty()){
            yyget_extra(yyscanner)->latentTokens.pop();
        }
        yyless(0);
        lexPopState(yyscanner);
    }

    .                               {
        yyless(0);
        if(yyget_extra(yyscanner)->indents.top()->start == -1){
            yyget_extra(yyscanner)->pendingEndInstr = false;
            yyget_extra(yyscanner)->indents.top()->start = yyget_extra(yyscanner)->lineIndents;
            yyget_extra(yyscanner)->indents.top()->current = yyget_extra(yyscanner)->lineIndents;
        }
        else if(yyget_extra(yyscanner)->lineIndents == yyget_extra(yyscanner)->indents.top()->current){
            if(yyget_extra(yyscanner)->pendingEndInstr){
                yyget_extra(yyscanner)->pendingEndInstr = false;
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
            }
        }
        else if(yyget_extra(yyscanner)->lineIndents > yyget_extra(yyscanner)->indents.top()->current){
            yyget_extra(yyscanner)->pendingEndInstr = false;
            yyget_extra(yyscanner)->tokens.push(TOK_INDENT);
            ++yyget_extra(yyscanner)->indents.top()->current;
            while(yyget_extra(yyscanner)->lineIndents > yyget_extra(yyscanner)->indents.top()->current){
                ++(yyget_extra(yyscanner)->indents.top()->current);
                yyget_extra(yyscanner)->tokens.push(TOK_BAD_INDENT);
            }
        }
        else{
            yyget_extra(yyscanner)->pendingEndInstr = false;
            while(yyget_extra(yyscanner)->lineIndents < yyget_extra(yyscanner)->indents.top()->current){
                --yyget_extra(yyscanner)->indents.top()->current;
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
                yyget_extra(yyscanner)->tokens.push(TOK_OUTDENT);
            }
            LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
            yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
        }
        lexPopState(yyscanner);
        if(!yyget_extra(yyscanner)->tokens.empty()){
            lexPushState(lex_state_unstack, yyscanner);
        }
    }
}

<lex_state_skipnl>{
    [ \t]+(\r\n|\r|\n)   { /* ignoring blank line */
        yyless(yyleng-1);
    }

    [\r\n][^\r\n]*       {
        ++yylineno;
        yylloc_param->first_column = 0;
        yyget_extra(yyscanner)->currentLine = "";
        yyget_extra(yyscanner)->currentLine.append(yytext+1, yyleng-1);
        LOG_LEXER("copying line 5: %s", yyget_extra(yyscanner)->currentLine.c_str());
        yyless(1);
    }

    .                               {
        yyless(0);
        lexPopState(yyscanner);
    }
}

<lex_state_spaceaware>{
    [ \t]*(\r\n|\r|\n)   { /* ignoring blank line */
        yyless(0);
        yyget_extra(yyscanner)->pendingEndInstr = true;
        yyget_extra(yyscanner)->lineIndents = 0;
        lexPopState(yyscanner);
    }

    "("                             {
        yyget_extra(yyscanner)->tokens.push(TOK_NO_SPACE);
        yyget_extra(yyscanner)->tokens.push(TLPAREN);
        lexPopState(yyscanner);
        lexPushState(lex_state_unstack, yyscanner);
    }

    "++"                             {
        yyget_extra(yyscanner)->tokens.push(TOK_NO_SPACE);
        yyget_extra(yyscanner)->tokens.push(TPLUSPLUS);
        lexPushState(lex_state_unstack, yyscanner);
    }

    "--"                             {
        yyget_extra(yyscanner)->tokens.push(TOK_NO_SPACE);
        yyget_extra(yyscanner)->tokens.push(TMINUSMINUS);
        lexPushState(lex_state_unstack, yyscanner);
    }

    [a-z_][a-zA-Z0-9_\?!]*           {
        yyget_extra(yyscanner)->tokens.push(TOK_NO_SPACE);
        yyget_lval(yyscanner)->string = new std::string(strdup(yytext), yyleng); \
        yyget_extra(yyscanner)->tokens.push(TIDENTIFIER);
        lexPushState(lex_state_unstack, yyscanner);
    }

    .                                 {
        yyless(0);
        lexPopState(yyscanner);
    }
}

<lex_state_unstack>{
    "\r\n"           {
        yyless(1);
    }
    [\r\n]          {
        yyless(0);
        if(yyget_extra(yyscanner)->tokens.empty()){
            lexPopState(yyscanner);
        }
        else{
            int result = yyget_extra(yyscanner)->tokens.front();
            yyget_extra(yyscanner)->tokens.pop();
            return result;
        }
    }

    .                   {
        yyless(0);
        if(yyget_extra(yyscanner)->tokens.empty()){
            lexPopState(yyscanner);
        }
        else{
            int result = yyget_extra(yyscanner)->tokens.front();
            yyget_extra(yyscanner)->tokens.pop();
            return result;
        }
    }
}



<lex_state_normal>{
    "\r\n"                        {
        yyless(1);
    }
    [\r\n][^\r\n]*                     {
        ++yylineno;
        yylloc_param->first_column = 0;
        yyget_extra(yyscanner)->currentLine = "";
        yyget_extra(yyscanner)->currentLine.append(yytext+1, yyleng-1);
        LOG_LEXER("copying line 3: %s", yyget_extra(yyscanner)->currentLine.c_str());
        yyless(1);
#if INTERACTIVE_INPUT
    if(yyget_extra(yyscanner)->interactive){
        return TOK_END_INSTR;
    }
    else {
#endif
        yyget_extra(yyscanner)->pendingEndInstr = true;
        yyget_extra(yyscanner)->lineIndents = 0;
        lexPushState(lex_state_newline, yyscanner);
#if INTERACTIVE_INPUT
    }
#endif
    }
    [\t ]+                      { yylloc_param->first_column += yyleng; }
    "//"                        { yylloc_param->first_column += yyleng; lexPushState(lex_state_skipline, yyscanner); }
    \#[ \t]                     { yylloc_param->first_column += yyleng; lexPushState(lex_state_skipline, yyscanner); }
    \/\*+                       {
        yylloc_param->first_column += yyleng;
        lexPushState(lex_state_mlcomment, yyscanner);
        yyget_extra(yyscanner)->commentStartCount = yyleng -1;
    }
    "return"                    { return TRETURN; }
    "if"                        { return TIF; }
    "else"                      { return TELSE; }
    "true"                      { return TTRUE; }
    "false"                     { return TFALSE; }
    "and"                       { return TAND; }
    "&&"                        { return TAND; }
    "or"                        { return TOR;  }
    "while"                     { return TWHILE; }
    "||"                        { return TOR;  }
    [A-Z][a-zA-Z0-9_\?]*        { yyReturnId(TTYPENAME, yyscanner); }
    [a-z_][a-zA-Z0-9_\?!]*      { lexPushState(lex_state_spaceaware, yyscanner); yyReturnId(TIDENTIFIER, yyscanner); }
    [0-9]+\.[0-9]*              { yyReturnId(TDOUBLE, yyscanner); }
    [0-9]+                      { yyReturnId(TINTEGER, yyscanner); }
    "="                         { return TEQUAL; }
    "=="                        { yyReturnId(TCEQ, yyscanner); }
    "!="                        { yyReturnId(TCNE, yyscanner); }
    "<"                         { yyReturnId(TCLT, yyscanner); }
    "<="                        { yyReturnId(TCLE, yyscanner); }
    ">"                         { yyReturnId(TCGT, yyscanner); }
    ">="                        { yyReturnId(TCGE, yyscanner); }
    "."                         { return TDOT; }
    ","                         { lexPushState(lex_state_skipnl, yyscanner); return TCOMMA; }
    "++"                        { lexPushState(lex_state_spaceaware, yyscanner); return TPLUSPLUS; }
    "--"                        { lexPushState(lex_state_spaceaware, yyscanner); return TMINUSMINUS; }
    "+"                         { lexPushState(lex_state_skipnl, yyscanner); yyReturnId(TPLUS, yyscanner); }
    "-"                         { lexPushState(lex_state_skipnl, yyscanner); yyReturnId(TMINUS, yyscanner); }
    "*"                         { lexPushState(lex_state_skipnl, yyscanner); yyReturnId(TMUL, yyscanner); }
    "/"                         { lexPushState(lex_state_skipnl, yyscanner); yyReturnId(TDIV, yyscanner); }
    ";"                         { return TOK_END_INSTR; }
    ":"                         { return TCOLON; }
    "("                         { return TLPAREN; }
    ")"                         { return TRPAREN; }
    "{"                         {
        while(!yyget_extra(yyscanner)->latentTokens.empty()){
            yyget_extra(yyscanner)->latentTokens.pop();
        }
        yyget_extra(yyscanner)->indents.push(new IndentContext());
        yyget_extra(yyscanner)->indents.top()->start = -1;
        yyget_extra(yyscanner)->indents.top()->current = -1;
        return TLBRACE;
    }
    "}"                         {
        ++yylloc_param->first_column;
        LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
        yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
        if(yyget_extra(yyscanner)->indents.top()->start != -1){
            while(yyget_extra(yyscanner)->indents.top()->current > yyget_extra(yyscanner)->indents.top()->start){
                --yyget_extra(yyscanner)->indents.top()->current;
                yyget_extra(yyscanner)->tokens.push(TOK_OUTDENT);
                LOG_LEXER(" ## adding TOK_END_INSTR (line %d)", __LINE__);
                yyget_extra(yyscanner)->tokens.push(TOK_END_INSTR);
            }
        }

        yyget_extra(yyscanner)->tokens.push(TRBRACE);
        lexPushState(lex_state_rbrace, yyscanner);
        lexPushState(lex_state_unstack, yyscanner);
        delete yyget_extra(yyscanner)->indents.top();
        yyget_extra(yyscanner)->indents.pop();
    }

    .                         {
        yyerror(yylloc_param, yyget_extra(yyscanner), "parsing failed: unknown token");
        return UNKNOWN_TOKEN;
    }
}

%%
