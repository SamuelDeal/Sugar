#include "ReturnStmt.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* ReturnStmt::create(Node *expression, YYLTYPE yyloc) {
    return new Node(Node::eReturnStmt, new ReturnStmt(expression), yyloc);
}

ReturnStmt::ReturnStmt(Node *expressionArg) {
    expression = expressionArg;
}

ReturnStmt::~ReturnStmt() {
    delete expression;
}

} // namespace ast
} // namespace sugar
