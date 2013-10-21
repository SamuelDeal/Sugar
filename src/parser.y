%{

#include <cstdio>
#include <cstdlib>
#include <stack>

#include "config.h"

#include "ast/all.h"

#define YY_HEADER_EXPORT_START_CONDITIONS 1

extern "C"  {

#include "lexer.h"

}

using namespace sugar;

extern ast::Block *programBlock;
#include "lexer_ctx.h"

extern LexerContext lexerCtx;
void yyerror(const char *s);
extern void onMainStatement(ast::Statement *);


#define lexIndentOn()
        /*yyIndentSensitiveStack.push(true); \
        LOG_LEXER("Indent sensitive ON"); \
        yyNLFound = false; */

#define lexIndentOff()
        /*yyIndentSensitiveStack.push(false); \
        LOG_LEXER("Indent sensitive OFF");*/

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
        ast::Comparison *comp;
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
/*%glr-parser*/

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TINTEGER TDOUBLE TTYPENAME
%token <token> TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TOK_INDENT TOK_BAD_INDENT TOK_OUTDENT TOK_END_INSTR TOK_NO_SPACE
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
%type <expr_list> func_call_args
%type <comp> comp_diff comp_eq comp_less comp_more
%type <expr> value expr func_call if_expr if if_else
%type <block> program block_stmts program_stmts block

/* Operator precedence for mathematical operators */


%left IF_ALONE
%left IF_ELSE
%left TELSE
%left BLOCK
%nonassoc TOK_END_INSTR
%left LOOSE_FUNC_CALL
%left FUNC_CALL_ARG
%left CALL_IMPL
%left CALL_EXPL
%left LOOSE_COMP
%left TCEQ
%nonassoc TCNE
%left TCLT TCLE
%left TCGT TCGE
%left TCOMMA
%left TIF
%left TPLUS TMINUS
%left TMUL TDIV


%start program

%%

program         : program_stmts { }
                ;

program_stmts   : program_stmt { onMainStatement($<stmt>1); }
                | program_stmts program_stmt { onMainStatement($<stmt>2); }
                | program_stmts TOK_END_INSTR {}
                ;

block_stmts     : stmt TOK_END_INSTR { $$ = new ast::Block(); $$->stmts.push_back($<stmt>1); }
                | block_stmts stmt TOK_END_INSTR { $1->stmts.push_back($<stmt>2); }
                ;

stmt            : action_stmt { $$ = $1; }
                | var_decl { $$ = $1; }
                | if { $$ = new ast::ExpressionStmt($1); }
                ;

action_stmt     : expr { $$ = new ast::ExpressionStmt($1); }
                | TRETURN expr { $$ = new ast::ReturnStmt($2); }
                | ident TEQUAL expr { $$ = new ast::Assignment($<ident>1, $3); }
                ;

program_stmt    : stmt TOK_END_INSTR { $$ = $1; }
                | func_decl TOK_END_INSTR { $$ = $1; }
                ;

expr			: expr TMUL expr { $$ = makeOperatorCall($1, TMUL, $3); }
                | expr TDIV expr { $$ = makeOperatorCall($1, TDIV, $3); }
                | expr TPLUS expr { $$ = makeOperatorCall($1, TPLUS, $3); }
                | expr TMINUS expr { $$ = makeOperatorCall($1, TMINUS, $3); }
                | func_call { $$ = $1; }
                | TLPAREN expr TRPAREN { $$ = $2; }
                | value { $$ = $1; }
                | ident %prec LOOSE_FUNC_CALL { $$ = $1; /* WARNING: var or implicit function call without args */ }
                | comp_diff %prec LOOSE_COMP{ $$ = $1; }
                | comp_eq %prec LOOSE_COMP{ $$ = $1; }
                | comp_less %prec LOOSE_COMP{ $$ = $1; }
                | comp_more %prec LOOSE_COMP{ $$ = $1; }
                | if_else { $$ = $1; }
                ;

comp_diff       : expr TCNE expr { $$ = new ast::Comparison($1); $$->add(TCNE, $3); }
                ;

comp_eq         : expr TCEQ expr { $$ = new ast::Comparison($1); $$->add(TCEQ, $3); }
                | comp_eq TCEQ expr  { $1->add(TCEQ, $3); $$ = $1; }
                ;

comp_less       : expr TCLT expr  { $$ = new ast::Comparison($1); $$->add(TCLT, $3); }
                | expr TCLE expr { $$ = new ast::Comparison($1); $$->add(TCLE, $3); }
                | comp_less TCLT expr { $1->add(TCLT, $3); $$ = $1; }
                | comp_less TCLE expr { $1->add(TCLE, $3); $$ = $1; }
                ;

comp_more       : expr TCGT expr { $$ = new ast::Comparison($1); $$->add(TCGT, $3); }
                | expr TCGE expr { $$ = new ast::Comparison($1); $$->add(TCGE, $3); }
                | comp_more TCGT expr  { $1->add(TCGT, $3); $$ = $1; }
                | comp_more TCGE expr{ $1->add(TCGE, $3); $$ = $1; }
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

block           : TLBRACE block_stmts TRBRACE { $$ = $2; }
                | TLBRACE TRBRACE { $$ = new ast::Block(); }
                | TOK_INDENT block_stmts TOK_OUTDENT { $$ = $2; }
                ;

func_decl       : typename ident TOK_NO_SPACE TLPAREN func_decl_args TRPAREN block { $$ = new ast::FunctionDeclaration($1, $2, $5, $7); }
                ;

func_decl_args  : /*blank*/  { $$ = new std::list<ast::VariableDeclaration*>(); }
                | var_decl { $$ = new std::list<ast::VariableDeclaration*>(); $$->push_back($<var_decl>1); }
                | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
                ;

if_expr         : TIF expr { $$ = $2; lexIndentOn(); }
                ;

if              : if_expr block %prec IF_ALONE { $$ = new ast::IfExpression($1, $2); }
                | action_stmt if_expr { ast::Block* tmp = new ast::Block(); tmp->stmts.push_back($<stmt>1); $$ = new ast::IfExpression($2, tmp); }
                ;

if_else         : if_expr block else block { $$ = new ast::IfExpression($1, $2, $4); }
                ;

else            : TELSE { lexIndentOn(); }
                ;

value           : TINTEGER { $$ = new ast::Constant(atoll($1->c_str())); delete $1; }
                | TDOUBLE { $$ = new ast::Constant(atof($1->c_str())); delete $1; }
                | TTRUE { $$ = new ast::Constant(true); }
                | TFALSE { $$ = new ast::Constant(false); }
                ;

%%

void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
