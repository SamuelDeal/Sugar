#include <iostream>
#include <stdio.h>
#include "codegen.h"
#include "node.h"
#include "functionlist.h"
#include "parser.hpp"
#include "config.h"

//Unusual, but it's the cleanest way
#define INTERACTIVE_INPUT 0
#include "lexer.cpp"

using namespace std;

//extern int yyparse();
NBlock* programBlock = new NBlock();

int main(int argc, char **argv)
{
    if(argc != 1 && argc != 2){
        std::cout << "Usage: " << argv[0] << "[INPUT_FILE] OUTPUT_FILE" << std::endl;
        return 1;
    }

    yy_flex_debug = DEBUG_LEXER;
    yydebug = DEBUG_PARSER;
    if(argc > 2){
        yyin = fopen(argv[1],"r");
    }
    std::string output = argv[2];

    llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.
    InitializeNativeTarget();

    yyparse();

    CodeGenContext context;
    context.generateCode(*programBlock);

    /*InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmPrinters();
    InitializeAllAsmParsers();

    PassRegistry *Registry = PassRegistry::getPassRegistry();
    initializeCore(*Registry);
    initializeCodeGen(*Registry);
    initializeLoopStrengthReducePass(*Registry);
    initializeLowerIntrinsicsPass(*Registry);
    initializeUnreachableBlockElimPass(*Registry);
*/
    //cl::AddExtraVersionPrinter(TargetRegistry::printRegisteredTargetsForVersion);

    // Get the target specific parser.
    std::string Error;
    const Target *TheTarget = TargetRegistry::lookupTarget(MArch, TheTriple,
                                                           Error);
    if (!TheTarget) {
      errs() << argv[0] << ": " << Error;
      return 1;
    }

    std::string FeaturesStr;
    if (MAttrs.size()) {
      SubtargetFeatures Features;
      for (unsigned i = 0; i != MAttrs.size(); ++i)
        Features.AddFeature(MAttrs[i]);
      FeaturesStr = Features.getString();
    }

    CodeGenOpt::Level OLvl = CodeGenOpt::Default; //CodeGenOpt::Aggressive;

    return 0;
}

void onMainStatement(NStatement *stmt){
    programBlock->statements.push_back(stmt);
}





