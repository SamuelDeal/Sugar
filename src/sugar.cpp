#include <iostream>
#include <stdio.h>

#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>

#include "config.h"
#define INTERACTIVE_INPUT 0
#include "lexer.batch.hpp"

#include "gen/Generator.h"
#include "gen/GeneratedCode.h"


using namespace std;
using namespace sugar;

ast::Block* programBlock = new ast::Block();

int main(int argc, char **argv)
{
    yy_flex_debug = DEBUG_LEXER;
    yydebug = DEBUG_PARSER;
    if(argc > 1){
        yyin = fopen(argv[1],"r");
        if(yyin == NULL){
            std::cout << "Unable to open " << argv[1] << std::endl;
            return 1;
        }
    }

    yyparse();

    gen::Generator generator;
    gen::GeneratedCode *generated = generator.generate(programBlock);

#if DEBUG_IR
    std::cerr << "\n========= Generated IR =========\n";
    // Print the bytecode in a human-readable format
    //   to see if our program compiled properly
    llvm::PassManager pm;
    pm.add(llvm::createPrintModulePass(&llvm::errs()));
    pm.run(*(generated->getModule()));
    std::cerr << "\n========= End Generated IR =========\n";
#endif

#if DEBUG_GENERATOR
    std::cerr << "\n=========== Running code... =============\n";
#endif
    llvm::ExecutionEngine *ee = llvm::EngineBuilder(generated->getModule()).create();
    vector<llvm::GenericValue> noargs;
    llvm::GenericValue v = ee->runFunction(generated->getFunction(), noargs);
#if DEBUG_GENERATOR
    std::cerr << "\n=========== Code was run. ==============\n";
#endif

    return 0;
}

void onMainStatement(sugar::ast::Statement *stmt){
    programBlock->stmts.push_back(stmt);
}




