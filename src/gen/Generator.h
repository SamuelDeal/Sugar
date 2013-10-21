#ifndef SUGAR_GEN_GENERATOR_H
#define SUGAR_GEN_GENERATOR_H

#include <llvm/Module.h>
#include <llvm/IRBuilder.h>

#include "../core/Scope.h"
#include "../core/CastGraph.h"
#include "AbstractGenerator.h"

namespace sugar {

namespace ast {
    class Node;
    class Assignment;
    class Block;
    class Comparison;
    class Constant;
    class ExpressionStmt;
    class FunctionCall;
    class FunctionDeclaration;
    class Identifier;
    class IfExpression;
    class ReturnStmt;
    class VariableDeclaration;
    class TypeIdentifier;
    class Operator;
}

namespace gen {

class GeneratedCode;
class Generation;

class Generator : public AbstractGenerator
{
public:
    Generator();
    virtual ~Generator();

    virtual GeneratedCode* generate(ast::Block *block);

    virtual llvm::Value* generateAddIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateAddFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateSubIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateSubFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateMulIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateMulFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateDivFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateEqBoolBool(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateEqIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateNEqBoolBool(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateNEqIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateNEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateLessIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateLessFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateLessEqIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateLessEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateMoreIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateMoreFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateMoreEqIntInt(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateMoreEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen);
    virtual llvm::Value* generateIntToFloatCast(std::vector<llvm::Value*> values, Generation &gen);

protected:
    void initCore(Generation &gen);

    llvm::Value* parseNode(ast::Node *node, Generation &gen);
    virtual llvm::Value* parse(ast::Assignment *node, Generation &gen);
    virtual llvm::Value* parse(ast::Block *node, Generation &gen);
    virtual llvm::Value* parse(ast::Comparison *node, Generation &gen);
    virtual llvm::Value* parse(ast::Constant *node, Generation &gen);
    virtual llvm::Value* parse(ast::ExpressionStmt *node, Generation &gen);
    virtual llvm::Value* parse(ast::FunctionCall *node, Generation &gen);
    virtual llvm::Value* parse(ast::FunctionDeclaration *node, Generation &gen);
    virtual llvm::Value* parse(ast::Identifier *node, Generation &gen);
    virtual llvm::Value* parse(ast::TypeIdentifier *node, Generation &gen);
    virtual llvm::Value* parse(ast::Operator *node, Generation &gen);
    virtual llvm::Value* parse(ast::IfExpression *node, Generation &gen);
    virtual llvm::Value* parse(ast::ReturnStmt *node, Generation &gen);
    virtual llvm::Value* parse(ast::VariableDeclaration *node, Generation &gen);

    virtual llvm::Function* generatePrintfFunction(Generation &gen);
    virtual core::Function* generateEchoBoolFunction(llvm::Function* printfFn, Generation &gen);
    virtual core::Function* generateEchoIntFunction(llvm::Function* printfFn, Generation &gen);
    virtual core::Function* generateEchoDoubleFunction(llvm::Function* printfFn, Generation &gen);
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATOR_H
