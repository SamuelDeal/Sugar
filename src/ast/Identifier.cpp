#include "Identifier.h"

namespace sugar {
namespace ast {

Identifier::Identifier(std::string *name) : Expression(Node::eIdentifier) {
    this->name = name;
}

Identifier::~Identifier(){
    delete name;
}

} // namespace ast
} // namespace sugar
