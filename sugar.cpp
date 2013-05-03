#include <iostream>
#include <stdio.h>
#include "codegen.h"
#include "node.h"
#include "functionlist.h"
#include "parser.hpp"
#include "config.h"

//Unusual, but it's the cleanest way
#define INTERACTIVE_INPUT 0
#include "lexer.cpp"

using namespace std;

NBlock* programBlock = new NBlock();

int main(int argc, char **argv)
{
    yy_flex_debug = DEBUG_LEXER;
    yydebug = DEBUG_PARSER;
    if(argc > 1){
        yyin = fopen(argv[1],"r");
    }

    InitializeNativeTarget();

    yyparse();

    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();

    return 0;
}

void onMainStatement(NStatement *stmt){
    programBlock->statements.push_back(stmt);
}




