#include "GeneratedCode.h"

#include "../utils.h"
#include <llvm/Function.h>
#include <llvm/BasicBlock.h>

#include "Generation.h"


namespace sugar {
namespace gen {

GeneratedCode::GeneratedCode(llvm::Function *function, Generation *gen, ast::Block *block) {
    _function = function;
    _generation = gen;
    _block = block;
    _statementCount = 0;
    _ee = NULL;
}

GeneratedCode::~GeneratedCode(){
    if(_generation != NULL){
        delete _generation;
    }
/*  //Do we nedd to destroy llvm resources, how ?
    if(_function != NULL){
        delete _function;
    }
    if(_mainBlock != NULL){
        delete _block;
    }
    if(_ee != NULL){
        delete _ee;
    }
    */
}

llvm::Function* GeneratedCode::getFunction() const {
    return _function;
}

llvm::Module* GeneratedCode::getModule() const {
    return _generation == NULL ? NULL : _generation->getModule();
}

Generation* GeneratedCode::getGeneration() const {
    return _generation;
}

ast::Block* GeneratedCode::getBlock() const {
    return _block;
}

int GeneratedCode::getStatementCount() const {
    return _statementCount;
}

void GeneratedCode::setStatementCount(int value) {
    _statementCount = value;
}

llvm::ExecutionEngine* GeneratedCode::getExecutionEngine() const {
    return _ee;
}

void GeneratedCode::setExecutionEngine(llvm::ExecutionEngine *ee) {
    _ee = ee;
}

} // namespace gen
} // namespace sugar
