#ifndef SUGAR_GEN_PASS_ABSTRACTPASS_H
#define SUGAR_GEN_PASS_ABSTRACTPASS_H

namespace llvm {
    class Value;
}


namespace sugar {

namespace ast {
    class Node;
    class ArgumentDeclaration;
    class Assignment;
    class Block;
    class Comparison;
    class Constant;
    class FunctionCall;
    class FunctionDeclaration;
    class Identifier;
    class IfExpression;
    class ReturnStmt;
    class VariableDeclaration;
    class TypeIdentifier;
    class Operator;
    class WhileStmt;
}

namespace gen {

class Generation;

namespace pass {

class AbstractPass
{
public:
    AbstractPass();
    virtual ~AbstractPass();

    llvm::Value* parseNode(ast::Node *node, Generation &gen);

    virtual llvm::Value* parse(ast::Node *node, ast::ArgumentDeclaration *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::Assignment *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::Block *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::Comparison *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::Constant *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::FunctionCall *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::Identifier *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::Operator *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::IfExpression *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen) = 0;
    virtual llvm::Value* parse(ast::Node *node, ast::WhileStmt *data, Generation &gen) = 0;
};

} // namespace pass
} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_PASS_ABSTRACTPASS_H
