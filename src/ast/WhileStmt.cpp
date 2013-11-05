#include "WhileStmt.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* WhileStmt::create(Node *expr, Node *block, bool testFirst, YYLTYPE yyloc){
    return new Node(Node::eWhileStmt, new WhileStmt(expr, block, testFirst), yyloc);
}

Node* WhileStmt::create(Node *expr, Node *block, YYLTYPE yyloc){
    return new Node(Node::eWhileStmt, new WhileStmt(expr, block, false), yyloc);
}

WhileStmt::WhileStmt(Node *exprArg, Node *blockArg, bool testFirstArg) {
    cond = exprArg;
    block = blockArg;
    testFirst = testFirstArg;
}

WhileStmt::~WhileStmt() {
    delete cond;
    delete block;
}

} // namespace ast
} // namespace sugar
