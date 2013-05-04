#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <list>
#include <llvm/Value.h>
#include <llvm/Instruction.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;


typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Node {
public:
        virtual ~Node() {}
        virtual llvm::Value* codeGen(CodeGenContext& context) = 0;
};

class NExpression : public Node {
public:
        virtual llvm::Type* getType() = 0;
        virtual bool isImplicitFunctionCall() const { return false; }
};

class NStatement : public Node {
public:
    virtual bool isVarDeclaration(){ return false; }
    virtual bool isFunctionDeclaration(){ return false; }
    virtual NExpression* getExpression() { return NULL; }
};

class NInteger : public NExpression {
public:
        long long value;
        NInteger(long long value) : value(value) { }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType();
};

class NDouble : public NExpression {
public:
        double value;
        NDouble(double value) : value(value) { }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType();
};

class NIdentifier : public NExpression {
public:
        std::string name;
        NIdentifier(const std::string& name) : name(name) { _type = NULL; }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType() { return _type; }
protected:
        llvm::Type* _type;
};

class NMethodCall : public NExpression {
protected:
        llvm::Type* _type;
        bool _explicit;
public:
        const NIdentifier& id;
        ExpressionList arguments;
        NMethodCall(const NIdentifier& id, bool explicitCall, ExpressionList& arguments) :
                id(id), arguments(arguments) { _type = NULL; _explicit = explicitCall; }
        NMethodCall(const NIdentifier& id, bool explicitCall) :
                id(id) { _type = NULL; _explicit = explicitCall; }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType() { return _type; }
        virtual bool isImplicitFunctionCall() const { return !_explicit; }
};

class NBinaryOperator : public NExpression {
public:
        int op;
        NExpression& lhs;
        NExpression& rhs;
        NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
                lhs(lhs), rhs(rhs), op(op) { _type = NULL; }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType() { return _type; }
private:
        llvm::Value* generate(llvm::Instruction::BinaryOps ops, CodeGenContext& context) const;
        llvm::Type* _type;
};

class NBlock : public NExpression {
public:
        StatementList statements;
        NBlock() { }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType() { return _type; }
protected:
        llvm::Type* _type;
};

class NIf : public NExpression {
public:
        NExpression *cond;
        NBlock *thenBlk;
        NBlock *elseBlk;
        NIf(NExpression* cond, NBlock *thenBlk, NBlock *elseBlk = NULL) :
            cond(cond), thenBlk(thenBlk), elseBlk(elseBlk){ _type = NULL; }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType() { return _type; }
protected:
        llvm::Type* _type;
};

class NExpressionStatement : public NStatement {
public:
        NExpression& expression;
        NExpressionStatement(NExpression& expression) :
                expression(expression) { }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType() { return expression.getType(); }
        virtual bool isExpression() { return true; }
        virtual NExpression* getExpression() { return &expression; }
};

class NReturnStatement : public NStatement {
public:
        NExpression& expression;
        NReturnStatement(NExpression& expression) :
                expression(expression) { }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType() { return expression.getType(); }
};

class NAssignment : public NStatement {
public:
        NIdentifier& lhs;
        NExpression& rhs;
        NAssignment(NIdentifier& lhs, NExpression& rhs) :
                lhs(lhs), rhs(rhs) { }
        virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
        const NIdentifier& type;
        NIdentifier& id;
        NExpression *assignmentExpr;
        NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
                type(type), id(id) { }
        NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
                type(type), id(id), assignmentExpr(assignmentExpr) { }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual bool isVarDeclaration(){ return true; }
};

class NFunctionDeclaration : public NStatement {
public:
        const NIdentifier& type;
        const NIdentifier& id;
        VariableList arguments;
        NBlock& block;
        NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id,
                        const VariableList& arguments, NBlock& block) :
                type(type), id(id), arguments(arguments), block(block) { }
        virtual llvm::Value* codeGen(CodeGenContext& context);
        bool isFunctionDeclaration(){ return true; }
};

class NEqComp : public NExpression {
public:
        NEqComp(NExpression *left, NExpression *right) {list.push_back(left);list.push_back(right);}
        void push(NExpression *right) {list.push_back(right);}
        virtual llvm::Value* codeGen(CodeGenContext& context);
        virtual llvm::Type* getType();

protected:
        std::list<NExpression *> list;
};

#endif // NODE_H
