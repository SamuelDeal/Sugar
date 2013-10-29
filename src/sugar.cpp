#include <iostream>
#include <stdio.h>

#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>

#include "gen/Generator.h"
#include "gen/GeneratedCode.h"
#include "parser/BatchParser.h"
#include "ast/Block.h"


using namespace std;
using namespace sugar;



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


    ast::Block programBlock;
    parser::BatchParser parser;    
    parser.parse(file, programBlock);

    gen::Generator generator;
    gen::GeneratedCode *generated = generator.generate(&programBlock);

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
