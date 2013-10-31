%{

#include <cstdio>
#include <cstdlib>
#include <stack>

#include "../config_checked.h"

#define YY_HEADER_EXPORT_START_CONDITIONS 1

#include "lexer.h"

using namespace sugar;

void yyerror(YYLTYPE *locp, sugar::parser::LexerContext *lexerCtx, const char *err);


ast::Node* makeOperatorCall(ast::Node *subject, int operatorId, ast::Node *arg1){
    std::list<ast::Node *> *args = new std::list<ast::Node *>();
    args->push_back(subject);
    args->push_back(arg1);
    return ast::Operator::create(operatorId, args);
}

ast::Node* makeUnaryOperatorCall(ast::Node *subject, int operatorId, bool before){
    std::list<ast::Node *> *args = new std::list<ast::Node *>();
    args->push_back(subject);
    return ast::Operator::create(operatorId, args, before);
}

%}

/* Represents the many different ways we can access our data */
%union {
        ast::Node *block;
        ast::Node *expr;
        ast::Node *stmt;
        ast::Node *ident;
        ast::Node *type_ident;
        ast::Node *var_decl;
        ast::Node *comp;
        std::list<ast::Node*> *args_decl;
        std::list<ast::Node*> *expr_list;
        std::string *string;
        int token;
}

%code requires {

#include <iostream>
#include "../ast/all.h"

using namespace sugar;

namespace sugar{
namespace parser{
    class LexerContext;
}
}

}


%pure-parser
%locations
%defines
%parse-param { sugar::parser::LexerContext *lexerCtx }
%lex-param { void *scanner }
%error-verbose

%{
#define scanner lexerCtx->scanner
%}

/* Token list */
%token <string> TIDENTIFIER TINTEGER TDOUBLE TTYPENAME
%token <token> TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TOK_INDENT TOK_BAD_INDENT TOK_OUTDENT TOK_END_INSTR TOK_NO_SPACE
%token <token> TRETURN TIF TELSE TTRUE TFALSE
%token <token> TCEQ TPLUS TMINUS TMUL TDIV TCOLON TOR TAND TWHILE TMINUSMINUS TPLUSPLUS

/* Types */
%type <ident> ident
%type <type_ident> typename
%type <stmt> stmt var_decl func_decl program_stmt action_stmt while
%type <args_decl> func_decl_args
%type <expr_list> func_call_args
%type <comp> comp_diff comp_eq comp_less comp_more
%type <expr> value expr func_call if_expr if if_else
%type <block> program block_stmts program_stmts block pre_block

/* Precedences */
%nonassoc TCOLON
%left IF_ALONE
%left IF_ELSE
%left TELSE
%nonassoc COLON_EXPR
%left BLOCK
%nonassoc TOK_END_INSTR
%left LOOSE_FUNC_CALL
%left FUNC_CALL_ARG
%left CALL_IMPL
%nonassoc UNARY
%left CALL_EXPL
%left LOOSE_COMP
%left TOR
%left TAND
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

program_stmts   : program_stmt { lexerCtx->onProgramStmt($<stmt>1); }
                | program_stmts program_stmt { lexerCtx->onProgramStmt($<stmt>2); }
                | program_stmts TOK_END_INSTR {}
                ;

block_stmts     : stmt TOK_END_INSTR {
                    $$ = ast::Block::create();
                    ((ast::Block*)($$->data))->stmts.push_back($<stmt>1);
                }
                | block_stmts stmt TOK_END_INSTR { ((ast::Block*)($1->data))->stmts.push_back($<stmt>2); }
                ;

stmt            : action_stmt { $$ = $1; }
                | var_decl {
                    ast::VariableDeclaration *varDecl = ((ast::VariableDeclaration*)($1->data));
                    if(varDecl->assign == NULL){
                        $$ = $1;
                    }
                    else {
                        ast::Block *block = new ast::Block();
                        block->stmts.push_back($1);
                        std::string *varName = new std::string(*((ast::Identifier*)(varDecl->id->data))->name);
                        block->stmts.push_back(ast::Assignment::create(ast::Identifier::create(varName), varDecl->assign));
                        varDecl->assign = NULL;
                        $$ = new ast::Node(ast::Node::eBlock, block);
                    }
                }
                | if { $$ = $1; }
                | while { $$ = $1; }
                ;

action_stmt     : expr { $$ = $1; }
                | TRETURN expr { $$ = ast::ReturnStmt::create($2); }
                | ident TEQUAL expr { $$ = ast::Assignment::create($<ident>1, $3); }
                ;

program_stmt    : stmt TOK_END_INSTR { $$ = $1; }
                | func_decl TOK_END_INSTR { $$ = $1; }
                ;

expr			: expr TMUL expr { $$ = makeOperatorCall($1, TMUL, $3); }
                | expr TDIV expr { $$ = makeOperatorCall($1, TDIV, $3); }
                | expr TPLUS expr { $$ = makeOperatorCall($1, TPLUS, $3); }
                | expr TMINUS expr { $$ = makeOperatorCall($1, TMINUS, $3); }
                | expr TAND expr { $$ = makeOperatorCall($1, TAND, $3); }
                | expr TOR expr { $$ = makeOperatorCall($1, TOR, $3); }
                | ident TOK_NO_SPACE TPLUSPLUS %prec UNARY { $$ = makeUnaryOperatorCall($1, TPLUSPLUS, false); }
                | ident TOK_NO_SPACE TMINUSMINUS %prec UNARY { $$ = makeUnaryOperatorCall($1, TMINUSMINUS, false); }
                | TPLUSPLUS TOK_NO_SPACE ident %prec UNARY { $$ = makeUnaryOperatorCall($3, TPLUSPLUS, true); }
                | TMINUSMINUS TOK_NO_SPACE ident %prec UNARY { $$ = makeUnaryOperatorCall($3, TMINUSMINUS, true); }
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

comp_diff       : expr TCNE expr { $$ = ast::Comparison::create($1); ((ast::Comparison*)($$->data))->add(TCNE, $3); }
                ;

comp_eq         : expr TCEQ expr { $$ = ast::Comparison::create($1); ((ast::Comparison*)($$->data))->add(TCEQ, $3); }
                | comp_eq TCEQ expr  { ((ast::Comparison*)($1->data))->add(TCEQ, $3); $$ = $1; }
                ;

comp_less       : expr TCLT expr  { $$ = ast::Comparison::create($1); ((ast::Comparison*)($$->data))->add(TCLT, $3); }
                | expr TCLE expr { $$ = ast::Comparison::create($1); ((ast::Comparison*)($$->data))->add(TCLE, $3); }
                | comp_less TCLT expr { ((ast::Comparison*)($1->data))->add(TCLT, $3); $$ = $1; }
                | comp_less TCLE expr { ((ast::Comparison*)($1->data))->add(TCLE, $3); $$ = $1; }
                ;

comp_more       : expr TCGT expr {
                    $$ = ast::Comparison::create($1);
                    ((ast::Comparison*)($$->data))->add(TCGT, $3);
                }
                | expr TCGE expr {
                    $$ = ast::Comparison::create($1);
                    ((ast::Comparison*)($$->data))->add(TCGE, $3);
                }
                | comp_more TCGT expr  {
                    ((ast::Comparison*)($1->data))->add(TCGT, $3);
                    $$ = $1;
                }
                | comp_more TCGE expr {
                    ((ast::Comparison*)($1->data))->add(TCGE, $3);
                    $$ = $1;
                }
                ;

func_call       : ident func_call_args %prec CALL_IMPL {
                    std::list<ast::Node*>::const_iterator it;
                    for (it = $2->begin(); it != $2->end(); it++) {
                        if((*it)->isImplicitFunctionCall()){
                            yyerror(&yyloc, lexerCtx, "implicit call forbidden here");
                        }
                    }
                    $$ = ast::FunctionCall::create($1, false, $2); }
                | ident TOK_NO_SPACE TLPAREN func_call_args TRPAREN %prec CALL_EXPL { $$ = ast::FunctionCall::create($1, true, $4); }
                | ident TOK_NO_SPACE TLPAREN TRPAREN { $$ = ast::FunctionCall::create($1, true, new std::list<ast::Node*>()); }
                ;

func_call_args  : expr %prec FUNC_CALL_ARG {
                    $$ = new std::list<ast::Node*>();
                    $$->push_back($1);
                }
                | func_call_args TCOMMA expr {
                    $1->push_back($3);
                    $$ = $1;
                }
                ;

typename        : TTYPENAME { $$ = ast::TypeIdentifier::create($1); }
                ;

ident           : TIDENTIFIER { $$ = ast::Identifier::create($1); }
                ;

var_decl        : typename ident { $$ = ast::VariableDeclaration::create($1, $2); }
                | typename ident TEQUAL expr { $$ = ast::VariableDeclaration::create($1, $2, $4); }
                ;

block           : pre_block { $$ = $1; }
                | TCOLON pre_block { $$ = $2; }
                | TCOLON expr %prec COLON_EXPR {
                    $$ = ast::Block::create();
                    ((ast::Block*)($$->data))->stmts.push_back($2);
                }
                ;

pre_block       : TLBRACE block_stmts TRBRACE { $$ = $2; }
                | TLBRACE TRBRACE { $$ = ast::Block::create(); }
                | TOK_INDENT block_stmts TOK_OUTDENT { $$ = $2; }
                ;

func_decl       : typename ident TOK_NO_SPACE TLPAREN func_decl_args TRPAREN block { $$ = ast::FunctionDeclaration::create($1, $2, $5, $7); }
                ;

func_decl_args  : /*blank*/  { $$ = new std::list<ast::Node*>(); }
                | var_decl {
                    $$ = new std::list<ast::Node*>();
                    $$->push_back(ast::ArgumentDeclaration::create((ast::VariableDeclaration*)($<var_decl>1->data)));
                }
                | func_decl_args TCOMMA var_decl {
                    $1->push_back(ast::ArgumentDeclaration::create((ast::VariableDeclaration*)($<var_decl>3->data)));
                    $$ = $1;
                }
                ;

if_expr         : TIF expr { $$ = $2; }
                ;

if              : if_expr block %prec IF_ALONE { $$ = ast::IfExpression::create($1, $2); }
                | action_stmt if_expr {
                    ast::Node* tmp = ast::Block::create();
                    ((ast::Block*)(tmp->data))->stmts.push_back($<stmt>1);
                    $$ = ast::IfExpression::create($2, tmp);
                }
                ;

if_else         : if_expr block else block { $$ = ast::IfExpression::create($1, $2, $4); }
                ;

else            : TELSE { }
                ;

while           : TWHILE expr block { $$ = ast::WhileStmt::create($2, $3); }
                ;

value           : TINTEGER { $$ = ast::Constant::create(atoll($1->c_str())); delete $1; }
                | TDOUBLE { $$ = ast::Constant::create(atof($1->c_str())); delete $1; }
                | TTRUE { $$ = ast::Constant::create(true); }
                | TFALSE { $$ = ast::Constant::create(false); }
                ;

%%
