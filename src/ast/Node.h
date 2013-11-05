#ifndef NODE_H
#define NODE_H

#include <list>
#include "../parser/parser.hpp"
#include "../utils/Getter.h"

namespace llvm {
    class Value;
}

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
        eFunctionImplementation,
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
    Node(Kind type, NodeData *data, YYLTYPE yylloc);
    virtual ~Node();
    Kind getKind() const;
    NodeData *data;

    virtual bool isImplicitFunctionCall() const;
    const core::Type* getType() const;
    void setType(const core::Type& type);
    void reset(Kind type, NodeData *data);
    YYLTYPE yylloc;

    void setRef(llvm::Value*);
    void setValue(utils::Getter<llvm::Value*> getter);
    llvm::Value *getValue();
    llvm::Value *getRef();

protected:
    Kind _kind;
    const core::Type *_type;
    utils::Getter<llvm::Value*> _value;
    llvm::Value* _ref;

};

} // namespace ast
} // namespace sugar

#endif // NODE_H
