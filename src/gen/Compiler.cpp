#include "Compiler.h"

#include <stdio.h>
#include <iostream>

#include <llvm/Support/TargetSelect.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>

#include "GeneratedCode.h"

namespace sugar {
namespace gen {

Compiler::Compiler() {
}

Compiler::~Compiler() {
}

bool Compiler::compile(GeneratedCode *genCode, const std::string &outputFile) const {
    std::string objectFilename = tmpnam(NULL);
    if(!generateObjectFile(genCode, objectFilename)){
        std::cout << "Unable to generate object file" << std::endl;
        return false;
    }

    if(!generateExecFile(objectFilename, outputFile)){
        std::cerr << "Unable to link executable" << std::endl;
        return false;
    }
    return true;
}


bool Compiler::generateObjectFile(GeneratedCode *genCode, const std::string &output) const {
    llvm::llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.

    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    std::string error;
    llvm::tool_output_file out(output.c_str(), error, llvm::raw_fd_ostream::F_Binary);
    if (!error.empty()) {
        return false;
    }
    llvm::formatted_raw_ostream fos(out.os());
    llvm::PassManager pm;

    llvm::InitializeAllTargetInfos();
    llvm::Triple triple(llvm::sys::getDefaultTargetTriple());
    const llvm::Target *targetLookup = llvm::TargetRegistry::lookupTarget("", triple, error);
    if (!targetLookup) {
      llvm::outs() << error;
      return false;
    }
    llvm::TargetOptions options;
    std::auto_ptr<llvm::TargetMachine> target(targetLookup->createTargetMachine(triple.getTriple(),
                "", "", options, llvm::Reloc::Default, llvm::CodeModel::Default, llvm::CodeGenOpt::Default));
    if ((*target).addPassesToEmitFile(pm, fos, llvm::TargetMachine::CGFT_ObjectFile, false, 0, 0)) {
      llvm::outs() << "target does not support generation of this file type!\n";
      return false;
    }

    pm.run(*genCode->getModule());

    out.keep();
    return true;
}

bool Compiler::generateExecFile(const std::string &objectFile, const std::string &out) const {
    std::string cmd = "gcc";
    cmd += " -o ";
    cmd += out;
    cmd += " ";
    cmd += objectFile;

    system(cmd.c_str());
    return true;
}

} // namespace gen
} // namespace sugar
