#include "Identifier.h"

#include "Node.h"

namespace sugar {
namespace ast {

Node* Identifier::create(std::string *name, YYLTYPE yylloc) {
    return new Node(Node::eIdentifier, new Identifier(name), yylloc);
}

Identifier::Identifier(std::string *name) {
    this->name = name;
}

Identifier::~Identifier(){
    delete name;
}

} // namespace ast
} // namespace sugar
