#include "AbstractPass.h"

#include "../../ast/Node.h"
#include <iostream>

namespace sugar {
namespace gen {
namespace pass {

AbstractPass::AbstractPass()
{
}

AbstractPass::~AbstractPass()
{
}

bool AbstractPass::parseNode(ast::Node *node, Generation &gen) {
    switch(node->getKind()){
        case ast::Node::eArgumentDeclaration:       return parse(node, (ast::ArgumentDeclaration *)node->data, gen);
        case ast::Node::eAssignment:                return parse(node, (ast::Assignment *)node->data, gen);
        case ast::Node::eBlock:                     return parse(node, (ast::Block *)node->data, gen);
        case ast::Node::eComparison:                return parse(node, (ast::Comparison *)node->data, gen);
        case ast::Node::eConstant:                  return parse(node, (ast::Constant *)node->data, gen);
        case ast::Node::eFunctionCall:              return parse(node, (ast::FunctionCall *)node->data, gen);
        case ast::Node::eFunctionImplementation:    return parse(node, (ast::FunctionImplementation *)node->data, gen);
        case ast::Node::eFunctionDeclaration:       return parse(node, (ast::FunctionDeclaration *)node->data, gen);
        case ast::Node::eIdentifier:                return parse(node, (ast::Identifier *)node->data, gen);
        case ast::Node::eIfExpression:              return parse(node, (ast::IfExpression *)node->data, gen);
        case ast::Node::eReturnStmt:                return parse(node, (ast::ReturnStmt *)node->data, gen);
        case ast::Node::eVariableDeclaration:       return parse(node, (ast::VariableDeclaration *)node->data, gen);
        case ast::Node::eTypeIdentifier:            return parse(node, (ast::TypeIdentifier *)node->data, gen);
        case ast::Node::eOperator:                  return parse(node, (ast::Operator *)node->data, gen);
        case ast::Node::eWhileStmt:                 return parse(node, (ast::WhileStmt *)node->data, gen);
        default:
            std::cout << "Unknown node type !!!";
            return false;
    }
}

} // namespace pass
} // namespace gen
} // namespace sugar
