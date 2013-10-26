#include <iostream>
#include <stdio.h>

#include "config.h"
#define INTERACTIVE_INPUT 0
#include "lexer.batch.hpp"

#include "gen/Compiler.h"
#include "gen/GeneratedCode.h"


using namespace std;
using namespace sugar;

ast::Block* programBlock = new ast::Block();
int main(int argc, char **argv)
{
    if(argc != 2 && argc != 3){
        std::cout << "Usage: " << argv[0] << "[INPUT_FILE] OUTPUT_FILE" << std::endl;
        return 1;
    }

    yyin = argc > 2 ? fopen(argv[1],"r") : stdin;
    if(yyin == NULL){
        std::cout << "Unable to open " << argv[1] << std::endl;
        return 1;
    }

    yyparse();

    gen::Compiler *compiler = new gen::Compiler();
    gen::GeneratedCode *genCode = compiler->generate(programBlock);
    if(!compiler->compile(genCode, argv[(argc == 2 ? 1 : 2)])){
        return 1;
    }
    return 0;
}

void onMainStatement(sugar::ast::Statement *stmt){
    programBlock->stmts.push_back(stmt);
}





