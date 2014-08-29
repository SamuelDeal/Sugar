#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "utils/config_checked.h"

#include "parser/InteractiveParser.h"

#include "gen/Interpreter.h"
#include "gen/GeneratedCode.h"

#include "ast/Node.h"
#include "parser/ProgramNode.h"
#include "ast/VariableDeclaration.h"
#include "ast/Assignment.h"
#include "core/Variable.h"

using namespace std;
using namespace sugar;

parser::ProgramNode* programStmts;
gen::Interpreter *interpreter;
bool runSucceed = true;
bool interactive = false;
#if SHELL_USE_COLOR
bool useColor = false;
#endif

void onMainStatement(parser::ProgramNode *programStmts, ast::Node *newStmt) {
    if((newStmt->getKind() == ast::Node::eFunctionDeclaration) ||
           (newStmt->getKind() == ast::Node::eVariableDeclaration)){
        programStmts->addStmt(newStmt);
    }
    else {
#if SHELL_USE_COLOR
        if(!interpreter->run(newStmt, programStmts, interactive, useColor)){
#else
        if(!interpreter->run(newStmt, programStmts, interactive)){
#endif
            runSucceed = false;
        }
    }
}

int main(int argc, char **argv) {
    FILE *file;
    std::string *filename = NULL;
    if(argc > 1){
        file = fopen(argv[1],"r");
        filename = new std::string(argv[1]);
        if(file == NULL){
            std::cout << "Unable to open " << argv[1] << std::endl;
            return 1;
        }
    }
    else{
        file = stdin;
    }

    interpreter = new gen::Interpreter();
    programStmts = new parser::ProgramNode(filename);

    interactive = (stdin == file);
#ifndef OS_WINDOWS
    interactive = interactive && (isatty(fileno(file)) == 1);
#if SHELL_USE_COLOR
    useColor = interactive && (isatty(fileno(stdout)) == 1);
#endif
#endif

    parser::InteractiveParser parser;
    bool succeed = parser.parse(file, filename, *programStmts, onMainStatement);

    delete programStmts;
    return succeed && (interactive || runSucceed) ? 0 : 1;
}


