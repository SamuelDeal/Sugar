#include "Block.h"

namespace sugar {
namespace ast {

Block::Block() : Statement(Node::eBlock)
{
}

Block::~Block() {
    for(std::list<Statement *>::iterator i = stmts.begin(); i != stmts.end(); i++){
       delete (*i);
    }
}

} // namespace ast
} // namespace sugar
