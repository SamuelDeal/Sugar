#include <iostream>
#include <stdio.h>

#include "ast/Block.h"
#include "parser/BatchParser.h"
#include "gen/Compiler.h"

using namespace std;
using namespace sugar;

ast::Block* programBlock = new ast::Block();
int main(int argc, char **argv)
{
    if(argc != 2 && argc != 3){
        std::cout << "Usage: " << argv[0] << "[INPUT_FILE] OUTPUT_FILE" << std::endl;
        return 1;
    }

    FILE *file = argc > 2 ? fopen(argv[1],"r") : stdin;
    if(file == NULL){
        std::cout << "Unable to open " << argv[1] << std::endl;
        return 1;
    }

    ast::Block programBlock;
    parser::BatchParser parser;
    parser.parse(file, programBlock);

    gen::Compiler compiler;
    bool compilSuccess = compiler.compile(programBlock, argv[(argc == 2 ? 1 : 2)]);
    return compilSuccess ? 0 : 1;
}
