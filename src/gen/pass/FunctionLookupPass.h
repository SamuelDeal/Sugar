#ifndef SUGAR_GEN_PASS_FUNCTIONLOOKUPPASS_H
#define SUGAR_GEN_PASS_FUNCTIONLOOKUPPASS_H

#include "AbstractPass.h"

namespace sugar {
namespace gen {
namespace pass {

class FunctionLookupPass : public AbstractPass
{
public:
    FunctionLookupPass();
    virtual ~FunctionLookupPass();

    virtual void parse(ast::Node *node, ast::ArgumentDeclaration *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::Assignment *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::Block *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::Comparison *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::Constant *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::FunctionCall *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::FunctionDeclaration *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::FunctionImplementation *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::Identifier *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::TypeIdentifier *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::Operator *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::IfExpression *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::ReturnStmt *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::VariableDeclaration *data, Generation &gen);
    virtual void parse(ast::Node *node, ast::WhileStmt *data, Generation &gen);
};

} // namespace pass
} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_PASS_FUNCTIONLOOKUPPASS_H
