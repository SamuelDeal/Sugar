#include <iostream>
#include <stdio.h>
#include "codegen.h"
#include "node.h"
#include "functionlist.h"
#include "parser.hpp"
#include "config.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/ManagedStatic.h"

//Unusual, but it's the cleanest way
#define INTERACTIVE_INPUT 0
#include "lexer.cpp"

using namespace std;

NBlock* programBlock = new NBlock();

bool parseSource(FILE* src, CodeGenContext &context){
    yyin =  src;
    yyparse();
    context.generateCode(*programBlock);
    return true;
}


bool generateObjectFile(CodeGenContext &context, const std::string &output){
    llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.

    InitializeAllTargetMCs();
    InitializeAllAsmPrinters();
    InitializeAllAsmParsers();

    std::string error;
    tool_output_file out(output.c_str(), error, raw_fd_ostream::F_Binary);
    if (!error.empty()) {
        return false;
    }
    formatted_raw_ostream fos(out.os());
    PassManager pm;

    InitializeAllTargetInfos();
    Triple triple(sys::getDefaultTargetTriple());
    const Target *targetLookup = TargetRegistry::lookupTarget("", triple, error);
    if (!targetLookup) {
      errs() << error;
      return false;
    }
    TargetOptions Options;
    std::auto_ptr<TargetMachine> target(targetLookup->createTargetMachine(triple.getTriple(),
                "", "", Options, Reloc::Default, CodeModel::Default, CodeGenOpt::Default));
    if ((*target).addPassesToEmitFile(pm, fos, TargetMachine::CGFT_ObjectFile, false, 0, 0)) {
      errs() << "target does not support generation of this file type!\n";
      return false;
    }

    pm.run(*context.module);

    out.keep();
    return true;
}

bool generateExecFile(const std::string &objectFile, const std::string &out){
    std::string cmd = "gcc";
    cmd += " -o ";
    cmd += out;
    cmd += " ";
    cmd += objectFile;

    system(cmd.c_str());
    return true;
}

int main(int argc, char **argv)
{
    if(argc != 2 && argc != 3){
        std::cout << "Usage: " << argv[0] << "[INPUT_FILE] OUTPUT_FILE" << std::endl;
        return 1;
    }

    yy_flex_debug = DEBUG_LEXER;
    yydebug = DEBUG_PARSER;
    std::string objectFilename = tmpnam(NULL);

    InitializeNativeTarget();
    CodeGenContext context;

    if(!parseSource(argc > 2 ? fopen(argv[1],"r") : stdin, context)){
        std::cerr << "Unable to parse source" << std::endl;
        return 1;
    }

    if(!generateObjectFile(context, objectFilename)){
        std::cerr << "Unable to generate object file" << std::endl;
        return 1;
    }

    if(!generateExecFile(objectFilename, argv[(argc == 2 ? 1 : 2)])){
        std::cerr << "Unable to link executable" << std::endl;
        return 1;
    }

    return 0;
}

void onMainStatement(NStatement *stmt){
    programBlock->statements.push_back(stmt);
}





