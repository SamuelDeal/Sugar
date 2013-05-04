#include "TypeIdentifier.h"

namespace sugar {
namespace ast {

TypeIdentifier::TypeIdentifier(std::string *name) : Node(Node::eTypeIdentifier) {
    this->name = name;
}

TypeIdentifier::~TypeIdentifier() {
    delete name;
}

} // namespace ast
} // namespace sugar
