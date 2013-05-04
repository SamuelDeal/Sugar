#include "FunctionCall.h"

#include "Identifier.h"

namespace sugar {
namespace ast {


FunctionCall::FunctionCall(Identifier *ident, bool explicitCall, std::list<Expression*> *expressions) :
    Expression(Node::eFunctionCall) {
    this->functionName = ident;
    this->args = expressions;
    _explicitCall = explicitCall;
}

FunctionCall::~FunctionCall(){
    for(std::list<Expression *>::iterator i = args->begin(); i != args->end(); i++){
       delete (*i);
    }
    delete args;
    delete functionName;
}

bool FunctionCall::isImplicitFunctionCall() const{
    return !_explicitCall;
}

} // namespace ast
} // namespace sugar
