#include <iostream>
#include <stdio.h>

#include "ast/Node.h"
#include "ast/Block.h"
#include "parser/BatchParser.h"
#include "gen/Compiler.h"

using namespace std;
using namespace sugar;

int main(int argc, char **argv) {
    if(argc != 2 && argc != 3){
        std::cout << "Usage: " << argv[0] << "[INPUT_FILE] OUTPUT_FILE" << std::endl;
        return 1;
    }

    FILE *file = argc > 2 ? fopen(argv[1],"r") : stdin;
    if(file == NULL){
        std::cout << "Unable to open " << argv[1] << std::endl;
        return 1;
    }

    ast::Node programStmts(ast::Node::eBlock, new ast::Block());
    parser::BatchParser parser;
    parser.parse(file, programStmts);

    gen::Compiler compiler;
    bool compilSuccess = compiler.compile(programStmts, argv[(argc == 2 ? 1 : 2)]);
    return compilSuccess ? 0 : 1;
}
