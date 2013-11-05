#include <iostream>
#include <stdio.h>

#include "config_checked.h"

#include "parser/InteractiveParser.h"

#include "gen/Interpreter.h"
#include "gen/GeneratedCode.h"

#include "ast/Node.h"
#include "ast/Block.h"
#include "ast/VariableDeclaration.h"
#include "ast/Assignment.h"
#include "core/Variable.h"

using namespace std;
using namespace sugar;

ast::Node* programStmts;
gen::Interpreter *interpreter;
bool interactive = false;
#if SHELL_USE_COLOR
bool useColor = false;
#endif

void onMainStatement(ast::Node *programStmts, ast::Node *newStmt) {
    if((newStmt->getKind() == ast::Node::eFunctionDeclaration) ||
           (newStmt->getKind() == ast::Node::eVariableDeclaration)){
        ((ast::Block*)(programStmts->data))->stmts->push_back(newStmt);
    }
    else {
#if SHELL_USE_COLOR
        interpreter->run(newStmt, programStmts, interactive, useColor);
#else
        interpreter->run(newStmt, programStmts, interactive);
#endif
    }
}

int main(int argc, char **argv) {
    FILE *file;
    if(argc > 1){
        file = fopen(argv[1],"r");
        if(file == NULL){
            std::cout << "Unable to open " << argv[1] << std::endl;
            return 1;
        }
    }
    else{
        file = stdin;
    }

    interpreter = new gen::Interpreter();
    YYLTYPE locStart;
    programStmts = ast::Block::create(locStart);

    interactive = (stdin == file);
#ifndef OS_WINDOWS
    interactive = interactive && (isatty(fileno(file)) == 1);
#if SHELL_USE_COLOR
    useColor = interactive && (isatty(fileno(stdout)) == 1);
#endif
#endif

    parser::InteractiveParser parser;
    parser.parse(file, *programStmts, onMainStatement);

    delete programStmts;
    return 0;
}


