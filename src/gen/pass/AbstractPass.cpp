#include "AbstractPass.h"

#include "../../ast/Node.h"

namespace sugar {
namespace gen {
namespace pass {

AbstractPass::AbstractPass()
{
}

AbstractPass::~AbstractPass()
{
}

void AbstractPass::parseNode(ast::Node *node, Generation &gen) {
    switch(node->getKind()){
        case ast::Node::eArgumentDeclaration:       parse(node, (ast::ArgumentDeclaration *)node->data, gen); break;
        case ast::Node::eAssignment:                parse(node, (ast::Assignment *)node->data, gen); break;
        case ast::Node::eBlock:                     parse(node, (ast::Block *)node->data, gen); break;
        case ast::Node::eComparison:                parse(node, (ast::Comparison *)node->data, gen); break;
        case ast::Node::eConstant:                  parse(node, (ast::Constant *)node->data, gen); break;
        case ast::Node::eFunctionCall:              parse(node, (ast::FunctionCall *)node->data, gen); break;
        case ast::Node::eFunctionImplementation:    parse(node, (ast::FunctionImplementation *)node->data, gen); break;
        case ast::Node::eFunctionDeclaration:       parse(node, (ast::FunctionDeclaration *)node->data, gen); break;
        case ast::Node::eIdentifier:                parse(node, (ast::Identifier *)node->data, gen); break;
        case ast::Node::eIfExpression:              parse(node, (ast::IfExpression *)node->data, gen); break;
        case ast::Node::eReturnStmt:                parse(node, (ast::ReturnStmt *)node->data, gen); break;
        case ast::Node::eVariableDeclaration:       parse(node, (ast::VariableDeclaration *)node->data, gen); break;
        case ast::Node::eTypeIdentifier:            parse(node, (ast::TypeIdentifier *)node->data, gen); break;
        case ast::Node::eOperator:                  parse(node, (ast::Operator *)node->data, gen); break;
        case ast::Node::eWhileStmt:                 parse(node, (ast::WhileStmt *)node->data, gen); break;
    }
}

} // namespace pass
} // namespace gen
} // namespace sugar
