#include "FunctionImplementation.h"


#include "Node.h"
#include "Block.h"

namespace sugar {
namespace ast {

FunctionImplementation::FunctionImplementation() {
    ifCount = 0;
}

FunctionImplementation::~FunctionImplementation() {
}

Node* FunctionImplementation::create(ast::Block* block, YYLTYPE yyloc){
    FunctionImplementation *data = new FunctionImplementation();
    data->stmts = block->stmts;
    block->clear();
    return new Node(Node::eFunctionImplementation, data, yyloc);
}

} // namespace ast
} // namespace sugar
