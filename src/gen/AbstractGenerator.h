#ifndef SUGAR_GEN_ABSTRACTGENERATOR_H
#define SUGAR_GEN_ABSTRACTGENERATOR_H

#include <vector>

namespace llvm{
    class Value;
}

namespace sugar {
namespace gen {

    class Generation;

class AbstractGenerator {
public:
    AbstractGenerator();
    virtual ~AbstractGenerator();

    virtual llvm::Value* generateAddIntInt(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateAddFloatFloat(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateSubIntInt(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateSubFloatFloat(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateMulIntInt(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateMulFloatFloat(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateDivFloatFloat(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateEqIntInt(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen) = 0;
    virtual llvm::Value* generateIntToFloatCast(std::vector<llvm::Value*> values, Generation &gen) = 0;
};

typedef llvm::Value* (AbstractGenerator::*NativeFunction)(std::vector<llvm::Value*>, Generation &gen);

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_ABSTRACTGENERATOR_H