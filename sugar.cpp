#include <iostream>
#include <stdio.h>
#include "codegen.h"
#include "node.h"
#include "functionlist.h"
#include "EditLineReader.h"
#include "parser.hpp"
#include <histedit.h>

//Unusual, but it's the cleanest way
#include "lexer.cpp"

using namespace std;

//extern int yyparse();
extern NBlock* programBlock;

EditLine *el;
History *hist;
HistEvent ev;

int yy_input_helper(char *buf, size_t max_size)
{
    const char *s;
    int count;

    s = el_gets(el, &count);
    if (count <= 0 || s == NULL)
        return 0;

    if ((size_t)count > max_size) {
        el_push(el, (char *)(s + max_size));
        count = max_size;
    }

    memcpy(buf, s, count);
    history(hist, &ev, H_ENTER, s);

    return count;
}

const char * prompt(EditLine *e){
    return "sugar> ";
}

int main(int argc, char **argv)
{
        char *progname = argv[0];
        hist = history_init();
        history(hist, &ev, H_SETSIZE, 1000);
        el = el_init(progname, stdin, stdout, stderr);
        el_set(el, EL_PROMPT, prompt);
        el_set(el, EL_SIGNAL, 1);
        el_set(el, EL_HIST, history, hist);
        el_set(el, EL_EDITOR, "emacs");

        printf("ascalc - A Simple Console Calculator\n");
        printf("Copyright (c) 2012-2013 Alex Hornung\n");
        printf("Type 'help' for available commands\n");
        printf("\n");


        yydebug = 1;

        if(argc > 1){
            yyin = fopen(argv[1],"r");
        }
        else{
            yyin = NULL;
        }
        //yyparse();

        //llvm::OwningPtr<IConsole> console();
        llvm::OwningPtr<ccons::LineReader> reader(new ccons::EditLineReader());

        InitializeNativeTarget();

        char *tok;
        tok = (char*)malloc(strlen("sugar> ")+1);
        tok = strcpy(tok, "sugar> ");

        char *tok2;
        tok2 = (char*)malloc(strlen("")+1);
        tok2 = strcpy(tok2, "");

        /*char *line = reader->readLine(tok, tok2);
        while (line) {
            YY_BUFFER_STATE bs = yy_scan_buffer((char*)line, sizeof(line));
            yy_switch_to_buffer(bs);
            yylex();
            delete line;
            line = reader->readLine(tok, tok2);
        }*/
        yyparse();
        printf("\n");
        /*const char *line = reader->readLine(console->prompt(), console->input());
        while (line) {
            //console->process(line);
            //line = reader->readLine(console->prompt(), console->input());
        }*/

        /*CodeGenContext context;
        context.generateCode(*programBlock);
        context.runCode();*/

        return 0;
}

void onMainStatement(){
    fprintf(stderr, "************ onMainStatement ***********");
    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();
}


