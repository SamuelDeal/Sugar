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
    class WhileStmt;
}

namespace gen {

class GeneratedCode;
class Generation;

class Generator : public AbstractGenerator
{
public:
    Generator();
    virtual ~Generator();

    virtual GeneratedCode* generate(ast::Block *block) const;

    virtual llvm::Value* generateAddIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateAddFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateSubIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateSubFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateMulIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateMulFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateDivFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateEqBoolBool(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateEqIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateNEqBoolBool(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateNEqIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateNEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateLessIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateLessFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateLessEqIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateLessEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateMoreIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateMoreFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateMoreEqIntInt(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateMoreEqFloatFloat(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateAndBoolBool(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateOrBoolBool(std::vector<llvm::Value*> values, Generation &gen) const;
    virtual llvm::Value* generateIntToFloatCast(std::vector<llvm::Value*> values, Generation &gen) const;

protected:
    void initCore(Generation &gen) const;

    llvm::Value* parseNode(ast::Node *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::Assignment *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::Block *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::Comparison *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::Constant *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::ExpressionStmt *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::FunctionCall *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::FunctionDeclaration *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::Identifier *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::TypeIdentifier *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::Operator *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::IfExpression *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::ReturnStmt *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::VariableDeclaration *node, Generation &gen) const;
    virtual llvm::Value* parse(ast::WhileStmt *node, Generation &gen) const;

    virtual llvm::Function* generatePrintfFunction(Generation &gen) const;
    virtual core::Function* generateEchoBoolFunction(llvm::Function* printfFn, Generation &gen) const;
    virtual core::Function* generateEchoIntFunction(llvm::Function* printfFn, Generation &gen) const;
    virtual core::Function* generateEchoDoubleFunction(llvm::Function* printfFn, Generation &gen) const;
};

} // namespace gen
} // namespace sugar

#endif // SUGAR_GEN_GENERATOR_H
