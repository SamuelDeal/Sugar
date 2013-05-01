#include <iostream>
#include <stdio.h>
#include "codegen.h"
#include "node.h"
#include "functionlist.h"
#include "parser.hpp"

//Unusual, but it's the cleanest way
#include "lexer.cpp"

using namespace std;

//extern int yyparse();
extern NBlock* programBlock;

int main(int argc, char **argv)
{
        yydebug = 1;
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

void onMainStatement(){
}


