#include "GeneratedCode.h"

#include "../utils.h"
#include <llvm/Function.h>
#include <llvm/BasicBlock.h>

#include "Generation.h"


namespace sugar {
namespace gen {

GeneratedCode::GeneratedCode(llvm::Function *function, Generation *gen, ast::Node *block) {
    _function = function;
    _generation = gen;
    _block = block;
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

ast::Node* GeneratedCode::getBlock() const {
    return _block;
}

} // namespace gen
} // namespace sugar
