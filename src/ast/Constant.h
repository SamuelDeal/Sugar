#ifndef SUGAR_AST_CONSTANT_H
#define SUGAR_AST_CONSTANT_H

#include "NodeData.h"
#include "../parser/parser.hpp"

namespace sugar {
namespace ast {

class Node;

class Constant : public NodeData
{
public:
    static Node* create(long long integerVal, YYLTYPE yyloc);
    static Node* create(double floatVal, YYLTYPE yyloc);
    static Node* create(bool boolVal, YYLTYPE yyloc);

    enum ConstantType {
        eBoolean,
        eInteger,
        eFloat
    };

    Constant(long long integerVal);
    Constant(double floatVal);
    Constant(bool boolVal);

    union {
        long long integerValue;
        double floatValue;
        bool boolValue;
    } value;

    ConstantType getConstantType() const;

protected:
    ConstantType _constantType;
};

} // namespace ast
} // namespace sugar

#endif // SUGAR_AST_CONSTANT_H
