#include "ProgramNode.h"

#include "../ast/Block.h"

namespace sugar {
namespace parser {

ProgramNode::ProgramNode(const std::string *filename) : Node(Node::eBlock, new ast::Block()) {
    yylloc.filename = filename;
    yylloc.first_line = 1;
    yylloc.last_line = 1;
    yylloc.first_column = 0;
    yylloc.last_column = 0;
}

ProgramNode::~ProgramNode(){
    if(yylloc.filename != NULL){
        delete yylloc.filename;
    }
}

void ProgramNode::addStmt(ast::Node *newStmt) {
    ((ast::Block *)(data))->stmts->push_back(newStmt);
}

} // namespace parser
} // namespace sugar
