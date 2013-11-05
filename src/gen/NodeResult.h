#ifndef SUGAR_GEN_NODERESULT_H
#define SUGAR_GEN_NODERESULT_H

#include <cstddef>

namespace llvm {
    class Value;
}

namespace sugar {
namespace gen {

class NodeResult
{
public:
    NodeResult(llvm::Value *val, llvm::Value *ref = NULL);
    NodeResult(const NodeResult &result);
    ~NodeResult();

    llvm::Value *val;
    llvm::Value *ref;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_NODERESULT_H
