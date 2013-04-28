%{

#include "node.h"
#include <cstdio>
#include <cstdlib>
NBlock *programBlock; /* the top level root node of our final AST */

//	extern int yylex();
//	void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }

extern "C" int yylex();
extern "C" FILE *yyin;

void yyerror(const char *s);

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
%token <token> TOK_INDENT TOK_BAD_INDENT TOK_OUTDENT TOK_NL TSEMICOL TOK_IMPLICIT_END_INSTR
%token <token> TRETURN

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident typename
%type <stmt> stmt var_decl func_decl program_stmt
%type <varvec> func_decl_args
%type <expr> numeric expr
%type <block> program block block_stmts program_stmts

/* Operator precedence for mathematical operators */
%left TMUL TDIV
%left TPLUS TMINUS

%start program

%%

program         : program_stmts { programBlock = $1; }
                ;

program_stmts   : program_stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
                | program_stmts program_stmt { $1->statements.push_back($<stmt>2); }
                ;

block_stmts     : /* empty */ { $$ = new NBlock(); }
                | stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
                | block_stmts stmt { $1->statements.push_back($<stmt>2); }
                ;

stmt            : expr sep { $$ = new NExpressionStatement(*$1); }
                | var_decl sep { $$ = $1; }
                | TRETURN expr { $$ = new NReturnStatement(*$2); }
                ;

program_stmt    : stmt { $$ = $1; }
                | func_decl sep { $$ = $1; }
                ;

block           : TLBRACE block_stmts TRBRACE { $$ = $2; }
                ;

numeric         : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
                | TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
                ;

expr            : numeric
                | expr TMUL expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr TDIV expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr TPLUS expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
                | expr TMINUS expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
                ;

typename        : TTYPENAME { $$ = new NIdentifier(*$1); delete $1; }
                ;

ident           : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
                ;

var_decl        : typename ident { $$ = new NVariableDeclaration(*$1, *$2); }
                ;

func_decl       : typename ident TLPAREN func_decl_args TRPAREN block { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
                ;

func_decl_args  : /*blank*/  { $$ = new VariableList(); }
                | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
                | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
                ;

sep             : TSEMICOL
                | TOK_NL
                | TOK_IMPLICIT_END_INSTR
                ;

%%

void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
