%{

#include <cstdio>
#include <cstdlib>
#include <stack>

#include "config.h"

#include "ast/all.h"

using namespace sugar;

extern ast::Block *programBlock; /* the top level root node of our final AST */

//	extern int yylex();
//	void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }

extern "C" int yylex();
extern "C" FILE *yyin;
extern "C" std::stack<bool> yyIndentSensitiveStack;

void yyerror(const char *s);
extern void onMainStatement(ast::Statement *);

#if defined(DEBUG_LEXER) && DEBUG_LEXER
#define LOG_LEXER(str, ...) fprintf(stderr, str, ##__VA_ARGS__)
#else
#define LOG_LEXER(str, ...)
#endif



ast::Operator* makeOperatorCall(ast::Expression *subject, int operatorId, ast::Expression *arg1){
    std::list<ast::Expression *> *args = new std::list<ast::Expression *>();
    args->push_back(subject);
    args->push_back(arg1);
    return new ast::Operator(operatorId, args);
}

%}

/* Represents the many different ways we can access our data */
%union {
        ast::Block *block;
        ast::Expression *expr;
        ast::Statement *stmt;
        ast::Identifier *ident;
        ast::TypeIdentifier *type_ident;
        ast::VariableDeclaration *var_decl;
        std::list<ast::VariableDeclaration*> *args_decl;
        std::list<ast::Expression*> *expr_list;
        std::string *string;
        int token;
}

%code requires {
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

#include "ast/all.h"

using namespace sugar;
}


%debug
%error-verbose
%glr-parser

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TINTEGER TDOUBLE TTYPENAME
%token <token> TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TOK_INDENT TOK_BAD_INDENT TOK_OUTDENT TOK_NL TSEMICOL TOK_IMPLICIT_END_INSTR TOK_NO_SPACE
%token <token> TRETURN TIF TELSE TTRUE TFALSE

%token <token> TCEQ TPLUS TMINUS TMUL TDIV

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <type_ident> typename
%type <stmt> stmt var_decl func_decl program_stmt action_stmt
%type <args_decl> func_decl_args
%type <expr_list> func_call_args comp_eq
%type <expr> value expr func_call if_expr if if_else
%type <block> program block_stmts program_stmts block

/* Operator precedence for mathematical operators */

%left IF_ALONE
%left IF_ELSE
%nonassoc TSEMICOL
%nonassoc TOK_NL
%nonassoc TOK_IMPLICIT_END_INSTR
%left TELSE
%left LOOSE_COMP
%left LOOSE_FUNC_CALL
%left FUNC_CALL_ARG
%left CALL_IMPL
%left CALL_EXPL
%left TCOMMA
%left TIF
%left TPLUS TMINUS
%left TMUL TDIV
%left TCEQ

%start program

%%

program         : program_stmts { }
                ;

program_stmts   : program_stmt { onMainStatement($<stmt>1); }
                | program_stmts program_stmt { onMainStatement($<stmt>2); }
                ;

block_stmts     : stmt sep { $$ = new ast::Block(); $$->stmts.push_back($<stmt>1); }
                | block_stmts stmt sep { $1->stmts.push_back($<stmt>2); }
                ;

stmt            : action_stmt { $$ = $1; }
                | var_decl { $$ = $1; }
                | if { $$ = new ast::ExpressionStmt($1); }
                ;

action_stmt     : expr { $$ = new ast::ExpressionStmt($1); }
                | TRETURN expr { $$ = new ast::ReturnStmt($2); }
                | ident TEQUAL expr { $$ = new ast::Assignment($<ident>1, $3); }
                ;

program_stmt    : stmt sep { $$ = $1; }
                | func_decl sep { $$ = $1; }
                ;

expr			: expr TMUL expr { $$ = makeOperatorCall($1, TMUL, $3); }
                | expr TDIV expr { $$ = makeOperatorCall($1, TDIV, $3); }
                | expr TPLUS expr { $$ = makeOperatorCall($1, TPLUS, $3); }
                | expr TMINUS expr { $$ = makeOperatorCall($1, TMINUS, $3); }
                | func_call { $$ = $1; }
                | TLPAREN expr TRPAREN { $$ = $2; }
                | value { $$ = $1; }
                | ident %prec LOOSE_FUNC_CALL { $$ = $1; /* WARNING: var or implicit function call without args */ }
                | comp_eq %prec LOOSE_COMP { $$ = new ast::Comparison(TCEQ, $<expr_list>1); }
                | if_else { $$ = $1; }
                ;

comp_eq         : expr TCEQ expr { $$ = new std::list<ast::Expression*>(); $$->push_back($1); $$->push_back($3); }
                | comp_eq TCEQ expr { $1->push_back($3); $$ = $1; }
                ;

func_call       : ident func_call_args %prec CALL_IMPL {
                    std::list<ast::Expression*>::const_iterator it;
                    for (it = $2->begin(); it != $2->end(); it++) {
                        if((*it)->isImplicitFunctionCall()){
                            yyerror("implicit call forbidden here");
                        }
                    }
                    $$ = new ast::FunctionCall($1, false, $2); }
                | ident TOK_NO_SPACE TLPAREN func_call_args TRPAREN %prec CALL_EXPL { $$ = new ast::FunctionCall($1, true, $4); }
                | ident TOK_NO_SPACE TLPAREN TRPAREN { $$ = new ast::FunctionCall($1, true, new std::list<ast::Expression*>()); }
                ;

func_call_args  : expr %prec FUNC_CALL_ARG { $$ = new std::list<ast::Expression*>(); $$->push_back($1); }
                | func_call_args TCOMMA expr { $1->push_back($3); $$ = $1; }
                ;

typename        : TTYPENAME { $$ = new ast::TypeIdentifier($1); }
                ;

ident           : TIDENTIFIER { $$ = new ast::Identifier($1); }
                ;

var_decl        : typename ident { $$ = new ast::VariableDeclaration($1, $2); }
                | typename ident TEQUAL expr { $$ = new ast::VariableDeclaration($1, $2, $4); }
                ;

block           : tlbrace block_stmts TRBRACE { $$ = $2; }
                | tlbrace TRBRACE { $$ = new ast::Block(); }
                | TOK_NL TOK_INDENT block_stmts TOK_OUTDENT { $$ = $3; yyIndentSensitiveStack.pop(); LOG_LEXER("Indent sensitive OFF"); }
                ;

tlbrace         : TLBRACE { yyIndentSensitiveStack.push(false); LOG_LEXER("Indent sensitive OFF"); }
                ;

func_decl       : typename ident TOK_NO_SPACE TLPAREN func_decl_args TRPAREN block { $$ = new ast::FunctionDeclaration($1, $2, $5, $7); }
                ;

func_decl_args  : /*blank*/  { $$ = new std::list<ast::VariableDeclaration*>(); yyIndentSensitiveStack.push(true); LOG_LEXER("Indent sensitive ON"); }
                | var_decl { $$ = new std::list<ast::VariableDeclaration*>(); $$->push_back($<var_decl>1); yyIndentSensitiveStack.push(true); LOG_LEXER("Indent sensitive ON"); }
                | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); yyIndentSensitiveStack.push(true); LOG_LEXER("Indent sensitive ON"); }
                ;

if_expr         : TIF expr { $$ = $2; yyIndentSensitiveStack.push(true); LOG_LEXER("Indent sensitive ON"); }
                ;

if              : if_expr block %prec IF_ALONE { $$ = new ast::IfExpression($1, $2); }
                | action_stmt if_expr { ast::Block* tmp = new ast::Block(); tmp->stmts.push_back($<stmt>1); $$ = new ast::IfExpression($2, tmp); }
                ;

if_else         : if_expr block else block { $$ = new ast::IfExpression($1, $2, $4); }
                | if_expr block sep else block %prec IF_ELSE { $$ = new ast::IfExpression($1, $2, $5); }
                ;

else            : TELSE { yyIndentSensitiveStack.push(true); LOG_LEXER("Indent sensitive ON"); }
                ;

value           : TINTEGER { $$ = new ast::Constant(atoll($1->c_str())); delete $1; }
                | TDOUBLE { $$ = new ast::Constant(atof($1->c_str())); delete $1; }
                | TTRUE { $$ = new ast::Constant(true); }
                | TFALSE { $$ = new ast::Constant(false); }
                ;

sep             : TSEMICOL
                | TOK_NL
                | TOK_IMPLICIT_END_INSTR
                ;

%%

void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
