#include <iostream>
#include <stdio.h>

#include "config.h"

#include "parser/InteractiveParser.h"

#include "gen/Interpreter.h"
#include "gen/GeneratedCode.h"

#include "ast/Block.h"
#include "ast/VariableDeclaration.h"
#include "ast/Assignment.h"

using namespace std;
using namespace sugar;

ast::Block* programBlock = new ast::Block();
gen::Interpreter *interpreter;

void onMainStatement(sugar::ast::Statement *stmt){
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
            interpreter->run(assign, programBlock);
        }
    }
    else {
        interpreter->run(stmt, programBlock);
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

    parser::InteractiveParser parser;
    parser.parse(file, *programBlock, onMainStatement);

    delete programBlock;
    return 0;
}


