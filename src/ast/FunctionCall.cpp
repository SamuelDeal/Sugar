#include "FunctionCall.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* FunctionCall::create(Node *ident, bool explicitCall, std::list<Node *> *expressions, YYLTYPE yyloc) {
    return new Node(Node::eFunctionCall, new FunctionCall(ident, explicitCall, expressions), yyloc);
}

FunctionCall::FunctionCall(Node *ident, bool explicitCall, std::list<Node*> *expressions) {
    this->functionName = ident;
    this->args = expressions;
    _explicitCall = explicitCall;
}

FunctionCall::~FunctionCall(){
    for(std::list<Node *>::iterator i = args->begin(); i != args->end(); i++){
       delete (*i);
    }
    delete args;
    delete functionName;
}

Identifier* FunctionCall::getFunctionName() const {
    return (Identifier*)(functionName->data);
}

bool FunctionCall::isImplicitFunctionCall() const{
    return !_explicitCall;
}

} // namespace ast
} // namespace sugar
