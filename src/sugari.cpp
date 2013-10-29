#include <iostream>
#include <stdio.h>

#include "config_checked.h"

#include "parser/InteractiveParser.h"

#include "gen/Interpreter.h"
#include "gen/GeneratedCode.h"

#include "ast/Block.h"
#include "ast/VariableDeclaration.h"
#include "ast/Assignment.h"
#include "core/Variable.h"

using namespace std;
using namespace sugar;

ast::Block* programBlock = new ast::Block();
gen::Interpreter *interpreter;
bool interactive = false;
#if SHELL_USE_COLOR
bool useColor = false;
#endif

void onMainStatement(ast::Block *programStmts, sugar::ast::Statement *stmt){
   if(stmt->getKind() == ast::Node::eFunctionDeclaration){
        programBlock->stmts.push_back(stmt);
    }
    else if(stmt->getKind() == ast::Node::eVariableDeclaration){
        ast::VariableDeclaration *stmtVar = (ast::VariableDeclaration*)stmt;
        if(stmtVar->assign == NULL){
            programBlock->stmts.push_back(stmt);
        }
        else{ //split assign and declaration
            ast::Assignment *assign = new ast::Assignment(stmtVar->id, stmtVar->assign);
            stmtVar->assign = NULL;
            programBlock->stmts.push_back(stmtVar);
#if SHELL_USE_COLOR
            interpreter->run(stmt, programBlock, interactive, useColor);
#else
            interpreter->run(stmt, programBlock, interactive);
#endif
        }
    }
    else {
#if SHELL_USE_COLOR
        interpreter->run(stmt, programBlock, interactive, useColor);
#else
        interpreter->run(stmt, programBlock, interactive);
#endif
    }
}

int main(int argc, char **argv)
{
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
    programBlock = new ast::Block();

    interactive = (stdin == file);
#ifndef OS_WINDOWS
    interactive = interactive && (isatty(fileno(file)) == 1);
#if SHELL_USE_COLOR
    useColor = interactive && (isatty(fileno(stdout)) == 1);
#endif
#endif

    parser::InteractiveParser parser;
    parser.parse(file, *programBlock, onMainStatement);

    delete programBlock;
    return 0;
}


