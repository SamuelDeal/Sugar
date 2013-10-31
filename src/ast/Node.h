#ifndef NODE_H
#define NODE_H

#include <list>

namespace sugar {

namespace core {
    class Type;
}

namespace ast {

class NodeData;

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
        eArgumentDeclaration,
        eComparison,
        eOperator,
        eWhileStmt
    };

public:
    Node(Kind type, NodeData *data);
    virtual ~Node();
    Kind getKind() const;
    NodeData *data;

    virtual bool isImplicitFunctionCall() const;
    const core::Type* getType() const;
    void setType(const core::Type& type);
    void reset(Kind type, NodeData *data);

protected:
    Kind _kind;
    const core::Type *_type;
};

} // namespace ast
} // namespace sugar

#endif // NODE_H
