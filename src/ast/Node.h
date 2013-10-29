#ifndef NODE_H
#define NODE_H

#include <list>

namespace sugar {

namespace core {
    class Type;
}

namespace ast {

class Node {
public:
    enum Kind {
        eConstant,
        eBlock,
        eIdentifier,
        eTypeIdentifier,
        eFunctionCall,
        eIfExpression,
        eReturnStmt,
        eAssignment,
        eVariableDeclaration,
        eFunctionDeclaration,
        eComparison,
        eExpressionStmt,
        eOperator,
        eWhileStmt
    };

protected:
    Node(Kind type);

public:
    virtual ~Node();

    Kind getKind() const;

    virtual bool isImplicitFunctionCall() const;
    const core::Type* getType() const;
    void setType(const core::Type& type);

protected:
    Kind _kind;
    const core::Type *_type;
};

} // namespace ast
} // namespace sugar

#endif // NODE_H
