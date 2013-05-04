#include "Assignment.h"

#include "Identifier.h"
#include "Expression.h"

namespace sugar {
namespace ast {

Assignment::Assignment(Identifier *left, Expression *right) : Statement(Node::eAssignment), left(left), right(right)
{
}

Assignment::~Assignment(){
    delete left;
    delete right;
}

} // namespace ast
} // namespace sugar
