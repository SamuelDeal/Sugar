#include <iostream>
#include <stdio.h>
#include "codegen.h"
#include "node.h"
#include "functionlist.h"

using namespace std;

extern int yyparse();
extern NBlock* programBlock;

extern FILE *yyin;
extern int yydebug;

int main(int argc, char **argv)
{

        yydebug = 1;

        if(argc > 1){
            yyin = fopen(argv[1],"r");
        }
        yyparse();
        // see http://comments.gmane.org/gmane.comp.compilers.llvm.devel/33877
        InitializeNativeTarget();
        CodeGenContext context;
        context.generateCode(*programBlock);
        context.runCode();

        return 0;
}


