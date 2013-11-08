#include <iostream>
#include <stdio.h>

#include "parser/ProgramNode.h"
#include "parser/BatchParser.h"
#include "gen/Compiler.h"

using namespace std;
using namespace sugar;

int main(int argc, char **argv) {
    if(argc != 2 && argc != 3){
        std::cout << "Usage: " << argv[0] << "[INPUT_FILE] OUTPUT_FILE" << std::endl;
        return 1;
    }

    FILE *file;
    std::string *filename = NULL;
    if(argc > 2){
        file = fopen(argv[1],"r");
        filename = new std::string(argv[1]);
    }
    else{
        file = stdin;
    }

    if(file == NULL){
        std::cout << "Unable to open " << argv[1] << std::endl;
        return 1;
    }

    parser::ProgramNode programStmts(filename);
    parser::BatchParser parser;
    if(!parser.parse(file, filename, programStmts)){
        return 1;
    }

    gen::Compiler compiler;
    bool compilSuccess = compiler.compile(programStmts, argv[(argc == 2 ? 1 : 2)]);
    return compilSuccess ? 0 : 1;
}
