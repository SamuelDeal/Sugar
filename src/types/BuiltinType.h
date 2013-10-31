#ifndef SUGAR_TYPES_BUILTINTYPE_H
#define SUGAR_TYPES_BUILTINTYPE_H

#include <vector>
#include "../gen/Generation.h"

namespace llvm {
    class Value;
}

namespace sugar {
namespace types {

class BuiltinType
{
public:
    BuiltinType();
    virtual ~BuiltinType();

    virtual void generate(gen::Generation &gen) const = 0;
};

} // namespace types
} // namespace sugar

#endif // SUGAR_TYPES_BUILTINTYPE_H
