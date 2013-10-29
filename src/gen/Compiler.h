#ifndef SUGAR_GEN_COMPILER_H
#define SUGAR_GEN_COMPILER_H

#include "Generator.h"

namespace sugar {
namespace gen {

class Compiler : public Generator
{
public:
    Compiler();
    virtual ~Compiler();

    bool compile(ast::Block &programStmts, const std::string &outputFile) const;

protected:
    bool generateObjectFile(GeneratedCode *genCode, const std::string &output) const;
    bool generateExecFile(const std::string &objectFile, const std::string &out) const;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_COMPILER_H
