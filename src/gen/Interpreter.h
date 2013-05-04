#ifndef SUGAR_GEN_INTERPRETER_H
#define SUGAR_GEN_INTERPRETER_H

#include "Generator.h"

namespace sugar {
namespace ast {
    class Statement;
}

namespace gen {

class Interpreter : public Generator
{
public:
    Interpreter();
    virtual ~Interpreter();

    virtual GeneratedCode* generate(ast::Block *block);

    void run(ast::Statement *stmt, GeneratedCode *genCode);
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_INTERPRETER_H
