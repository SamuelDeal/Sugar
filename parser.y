%{

#include "node.h"
#include <cstdio>
#include <cstdlib>
#include <stack>
extern NBlock *programBlock; /* the top level root node of our final AST */

//	extern int yylex();
//	void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }

extern "C" int yylex();
extern "C" FILE *yyin;
extern "C" std::stack<bool> yyIndentSensitiveStack;

void yyerror(const char *s);
extern void onMainStatement(NStatement *);

%}

/* Represents the many different ways we can access our data */
%union {
        Node *node;
        NBlock *block;
        NExpression *expr;
        NStatement *stmt;
        NIdentifier *ident;
        NVariableDeclaration *var_decl;
        std::vector<NVariableDeclaration*> *varvec;
        std::vector<NExpression*> *exprvec;
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
}


%debug
%error-verbose
%glr-parser

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TINTEGER TDOUBLE TTYPENAME
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TOK_INDENT TOK_BAD_INDENT TOK_OUTDENT TOK_NL TSEMICOL TOK_IMPLICIT_END_INSTR TOK_NO_SPACE
%token <token> TRETURN

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident typename
%type <stmt> stmt var_decl func_decl program_stmt
%type <varvec> func_decl_args
%type <exprvec> func_call_args func_call_args_expl
%type <expr> numeric expr expr_expl func_call_impl func_call_expl
%type <block> program block_stmts program_stmts block

/* Operator precedence for mathematical operators */

%nonassoc IMPLICIT_CALL

%left TCOMMA
%left CALL_ARGS
%left EXPR_EXPL
%left TPLUS TMINUS
%left TMUL TDIV


%start program

%%

program         : program_stmts { }
                ;

program_stmts   : program_stmt { onMainStatement($<stmt>1); }
                | program_stmts program_stmt { onMainStatement($<stmt>2); }
                ;

block_stmts     : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
                | block_stmts stmt { $1->statements.push_back($<stmt>2); }
                ;

stmt            : expr sep { $$ = new NExpressionStatement(*$1); }
                | var_decl sep { $$ = $1; }
                | TRETURN expr sep { $$ = new NReturnStatement(*$2); }
                | ident TEQUAL expr sep { $$ = new NAssignment(*$<ident>1, *$3); }
                ;

program_stmt    : stmt { $$ = $1; }
                | func_decl sep { $$ = $1; }
                ;

numeric         : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
                | TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
                ;

expr			: expr_expl %prec EXPR_EXPL { $$ = $1; }
                | expr TMUL expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr TDIV expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr TPLUS expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr TMINUS expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | func_call_impl { $$ = $1; }
                ;

expr_expl       : TLPAREN expr TRPAREN { $$ = $2; }
                | ident { $$ = new NMethodCall(*$1); /* WARNING: var or implicit function call without args */ }
                | numeric { $$ = $1; }
                | expr_expl TMUL expr_expl { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr_expl TDIV expr_expl { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr_expl TPLUS expr_expl { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr_expl TMINUS expr_expl { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | func_call_expl { $$ = $1; }
                ;

func_call_impl  : ident func_call_args_expl %prec IMPLICIT_CALL { $$ = new NMethodCall(*$1, *$2); delete $2; }
                ;

func_call_expl  : ident TOK_NO_SPACE TLPAREN func_call_args TRPAREN { $$ = new NMethodCall(*$1, *$4); delete $4; }
                | ident TOK_NO_SPACE TLPAREN TRPAREN { $$ = new NMethodCall(*$1); }
                ;

func_call_args  : expr { $$ = new ExpressionList(); $$->push_back($1); }
                | func_call_args TCOMMA expr { $1->push_back($3); }
                ;

func_call_args_expl     : expr_expl %prec CALL_ARGS { $$ = new ExpressionList(); $$->push_back($1); }
                        | func_call_args_expl TCOMMA expr_expl { $1->push_back($3); }
                        ;

typename        : TTYPENAME { $$ = new NIdentifier(*$1); delete $1; }
                ;

ident           : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; /* WARNING: var or implicit function call without args */ }
                ;

var_decl        : typename ident { $$ = new NVariableDeclaration(*$1, *$2); }
                | typename ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
                ;

block           : tlbrace block_stmts TRBRACE { $$ = $2; }
                | tlbrace TRBRACE { $$ = new NBlock(); }
                | TOK_NL TOK_INDENT block_stmts TOK_OUTDENT { $$ = $3; yyIndentSensitiveStack.pop(); fprintf(stderr, "Indent sensitive OFF\n"); }
                ;

tlbrace         : TLBRACE { yyIndentSensitiveStack.push(false); fprintf(stderr, "Indent sensitive OFF\n"); }
                ;

func_decl       : typename ident TOK_NO_SPACE TLPAREN func_decl_args TRPAREN block { $$ = new NFunctionDeclaration(*$1, *$2, *$5, *$7); delete $5; }
                ;

func_decl_args  : /*blank*/  { $$ = new VariableList(); yyIndentSensitiveStack.push(true); fprintf(stderr, "Indent sensitive ON\n"); }
                | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); yyIndentSensitiveStack.push(true); fprintf(stderr, "Indent sensitive ON\n"); }
                | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); yyIndentSensitiveStack.push(true); fprintf(stderr, "Indent sensitive ON\n"); }
                ;

sep             : TSEMICOL
                | TOK_NL
                | TOK_IMPLICIT_END_INSTR
                ;

%%

void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
