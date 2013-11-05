#include "Node.h"

#include "../utils.h"

#include "../core/Type.h"
#include "NodeData.h"
#include "../utils/Getter.tpl.h"

namespace sugar {
namespace ast {

Node::Node(Node::Kind kind, NodeData *dataArg, YYLTYPE yylloc) {
    _kind = kind;
    _type = NULL;
    _ref = NULL;
    data = dataArg;
    this->yylloc = yylloc;
}

Node::~Node() {
    delete data;
}

Node::Kind Node::getKind() const {
    return _kind;
}

bool Node::isImplicitFunctionCall() const{
    return false;
}

const core::Type* Node::getType() const {
    return _type;
}

void Node::setType(const core::Type& type) {
    _type = &type;
}


void Node::setRef(llvm::Value *ref){
    _ref = ref;
}

void Node::setValue(utils::Getter<llvm::Value*> getter) {
    _value = getter;
}

llvm::Value *Node::getValue(){
    if(!_value.isValid()){
        return NULL;
    }
    return _value;
}

llvm::Value *Node::getRef(){
    return _ref;
}

/*void Node::reset(Node::Kind kind, NodeData *dataArg) {
    _kind = kind;
    data = dataArg;
}*/

} // namespace ast
} // namespace sugar
