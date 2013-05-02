#include <sstream>

#include "node.h"
#include "codegen.h"
#include "parser.hpp"
#include "functionlist.h"
#include "config.h"

using namespace std;

void createCoreFunctions(CodeGenContext& context);
Value *returnValue = NULL;
GlobalValue * lastGlobal = NULL;

#if __cplusplus < 201103L //C++11
namespace std {

std::string to_string(int i){
    std::stringstream out;
    out << i;
    return out.str();
}

}
#endif

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock& root) {
#if DEBUG_GENERATOR
        std::cout << "Generating code...\n";
#endif

        // Create the top level interpreter function to call as entry
        vector<Type*> argTypes;
        FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), makeArrayRef(argTypes), false);
        mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
        BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

        createCoreFunctions(*this);

        /* Push a new variable/block context */
        pushBlock(bblock);
        root.codeGen(*this); /* emit bytecode for the toplevel block */
        ReturnInst::Create(getGlobalContext(), bblock);
        popBlock();


#if DEBUG_GENERATOR
        std::cout << "Code is generated.\n";
#endif
#if DEBUG_IR
        // Print the bytecode in a human-readable format
        //   to see if our program compiled properly
        PassManager pm;
        pm.add(createPrintModulePass(&outs()));
        pm.run(*module);
#endif
}

// Executes the AST by running the main function
GenericValue CodeGenContext::runCode() {
#if DEBUG_GENERATOR
        std::cout << "\n =========== Running code... ============= \n";
#endif
        ExecutionEngine *ee = EngineBuilder(module).create();
        vector<GenericValue> noargs;
        GenericValue v = ee->runFunction(mainFunction, noargs);
#if DEBUG_GENERATOR
        std::cout << "\n =========== Code was run. ==============\n";
#endif
        return v;
}

// Executes the AST by running the main function
GenericValue InteractiveCodeGenContext::runStatement(NStatement *stmt) {
    static int count = 0;
    ++count;

#if DEBUG_GENERATOR
    std::cout << "Generating code...\n";
#endif
    vector<Type*> argTypes;
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), makeArrayRef(argTypes), false);
    if(ee == NULL){
        createCoreFunctions(*this);
        ee = EngineBuilder(module).create();
    }

    while(stmtIndex < root.statements.size()){
        root.statements[stmtIndex]->codeGen(*this);
        ++stmtIndex;
    }

    Function *runFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "__shell_invoke__"+ std::to_string(count), module);
    BasicBlock *runBlock = BasicBlock::Create(getGlobalContext(), "entry", runFunction, 0);

    currentBlockOverride = runBlock;
    stmt->codeGen(*this);
    ReturnInst::Create(getGlobalContext(), runBlock);
    currentBlockOverride = NULL;

#if DEBUG_GENERATOR
    std::cout << "Code is generated.\n";
#endif
#if DEBUG_IR
    // Print the bytecode in a human-readable format
    //   to see if our program compiled properly
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*module);
#endif
#if DEBUG_GENERATOR
        std::cout << "\n =========== Running code... ============= \n";
#endif
    vector<GenericValue> noargs;
    GenericValue v = ee->runFunction(runFunction, noargs);
#if DEBUG_GENERATOR
        std::cout << "\n =========== Code was run. ==============\n";
#endif
    return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdentifier& type)
{
        if (type.name.compare("Int") == 0) {
                return Type::getInt64Ty(getGlobalContext());
        }
        else if (type.name.compare("Float") == 0) {
                return Type::getDoubleTy(getGlobalContext());
        }
        return Type::getVoidTy(getGlobalContext());
}

/* Returns an LLVM type based on the identifier */
static Constant *defaultValueOf(const NIdentifier& type)
{
        if (type.name.compare("Int") == 0) {
                return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0, true);
        }
        else if (type.name.compare("Float") == 0) {
                return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), 0);
        }
        return NULL;
}


/* -- Code Generation -- */

Value* NInteger::codeGen(CodeGenContext& context) {
#if DEBUG_GENERATOR
        std::cout << "Creating integer: " << value << endl;
#endif
        return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
}

Value* NDouble::codeGen(CodeGenContext& context)
{
#if DEBUG_GENERATOR
        std::cout << "Creating double: " << value << endl;
#endif
        return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value);
}

Value* NIdentifier::codeGen(CodeGenContext& context)
{
#if DEBUG_GENERATOR
        std::cout << "Creating identifier reference: " << name << endl;
#endif
        if (context.locals().find(name) == context.locals().end() && context.globals().find(name) == context.globals().end()) {
                std::cerr << "undeclared variable " << name << endl;
                return NULL;
        }
        return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}

Value* NMethodCall::codeGen(CodeGenContext& context)
{
#if DEBUG_GENERATOR
        std::cout << "\n*** Method call generation " << id.name << " \n";
#endif
        std::vector<Value*> args;
        ExpressionList::const_iterator it;
        for (it = arguments.begin(); it != arguments.end(); it++) {
                args.push_back((**it).codeGen(context));
        }

        std::list<Type *> types;
        for(std::vector<Value *>::const_iterator i = args.begin(); i != args.end(); i++){
            types.push_back((*i)->getType());
        }
        Function *function = core::functionList.getFunction(id.name, types);

        if (function == NULL) {
            Value *result = NULL;
            if (!context.isGlobalContext() && context.locals().find(id.name) != context.locals().end()) {
                result = new LoadInst(context.locals()[id.name], "", false, context.currentBlock());
            }
            else if(context.globals().find(id.name) != context.globals().end()){
                result = new LoadInst(context.globals()[id.name], "", false, context.currentBlock());
                //return context.globals()[id.name];
            }
            else {
                std::cerr << "No variable not function with that name variable " << id.name << endl;
            }
#if DEBUG_GENERATOR
            std::cout << "Creating ariable reference: " << id.name << endl;
#endif
            return result;
        }

        CallInst *call = CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
#if DEBUG_GENERATOR
        std::cout << "Creating method call: " << id.name << endl;
#endif
        return call;
}

Value* NBinaryOperator::codeGen(CodeGenContext& context)
{
        Value* lValue = lhs.codeGen(context);
        Value* rValue = rhs.codeGen(context);
#if DEBUG_GENERATOR
        std::cout << "Creating binary operation " << op << endl;
#endif
        if(lValue->getType() == Type::getInt64Ty(getGlobalContext()) && rValue->getType() == Type::getInt64Ty(getGlobalContext())){
            switch (op) {
                    case TPLUS: 	return BinaryOperator::Create(Instruction::Add, lValue, rValue, "", context.currentBlock());
                    case TMINUS: 	return BinaryOperator::Create(Instruction::Sub, lValue, rValue, "", context.currentBlock());
                    case TMUL: 		return BinaryOperator::Create(Instruction::Mul, lValue, rValue, "", context.currentBlock());
                    case TDIV: 		return BinaryOperator::Create(Instruction::SDiv, lValue, rValue, "", context.currentBlock());
            }
#if DEBUG_GENERATOR
            std::cerr << "unknown operand: " << op << std::endl;
#endif
        }
        else if(lValue->getType() == Type::getDoubleTy(getGlobalContext()) && rValue->getType() == Type::getDoubleTy(getGlobalContext())){
            switch (op) {
                    case TPLUS: 	return BinaryOperator::Create(Instruction::FAdd, lValue, rValue, "", context.currentBlock());
                    case TMINUS: 	return BinaryOperator::Create(Instruction::FSub, lValue, rValue, "", context.currentBlock());
                    case TMUL: 		return BinaryOperator::Create(Instruction::FMul, lValue, rValue, "", context.currentBlock());
                    case TDIV: 		return BinaryOperator::Create(Instruction::FDiv, lValue, rValue, "", context.currentBlock());
            }
            std::cerr << "unknown operand: " << op << std::endl;
        }
        std::cerr << "bad type conformance " << std::endl;
        return NULL;
}

Value* NBinaryOperator::generate(Instruction::BinaryOps ops, CodeGenContext& context) const {
    return BinaryOperator::Create(ops, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
}

Value* NAssignment::codeGen(CodeGenContext& context)
{
#if DEBUG_GENERATOR
        std::cout << "Creating assignment for " << lhs.name << endl;
#endif
        Value *toAssign = NULL;
        if (!context.isGlobalContext() && context.locals().find(lhs.name) != context.locals().end()) {
            toAssign = context.locals()[lhs.name];
        }
        else if(context.globals().find(lhs.name) != context.globals().end()){
            toAssign = context.globals()[lhs.name];
        }
        else{
            std::cerr << "undeclared variable " << lhs.name << endl;
            return NULL;
        }
        StoreInst *assign = new StoreInst(rhs.codeGen(context), toAssign, false, context.currentBlock());
        return assign;
}

Value* NBlock::codeGen(CodeGenContext& context)
{
        StatementList::const_iterator it;
        Value *last = NULL;
        for (it = statements.begin(); it != statements.end(); it++) {
#if DEBUG_GENERATOR
                std::cout << "Generating code for " << typeid(**it).name() << endl;
#endif
                last = (**it).codeGen(context);
        }
#if DEBUG_GENERATOR
        std::cout << "Creating block" << endl;
#endif
        return last;
}

Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
#if DEBUG_GENERATOR
        std::cout << "Generating code for " << typeid(expression).name() << endl;
#endif
        return expression.codeGen(context);
}

Value* NReturnStatement::codeGen(CodeGenContext &context)
{
#if DEBUG_GENERATOR
        std::cout << "Generating return value: " << typeid(expression).name() << endl;
#endif
        returnValue = expression.codeGen(context);
        return returnValue;
}

Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
        BasicBlock *currentBlock = context.currentBlock();
        if(currentBlock == NULL){ //global variable
#if DEBUG_GENERATOR
            std::cout << "Creating global variable declaration " << type.name << " " << id.name << endl;
#endif
            GlobalVariable *var = new GlobalVariable(*context.module, typeOf(type), false, GlobalValue::InternalLinkage, 0, id.name);
            var->setInitializer(defaultValueOf(type));
            context.globals()[id.name] = var;
            lastGlobal = var;
            return var;
        }
        else if(context.isGlobalContext()){ //use global context
            if (assignmentExpr != NULL) {
#if DEBUG_GENERATOR
                std::cout << "Assigning value to global variable declaration " << type.name << " " << id.name << endl;
#endif
                NAssignment assn(id, *assignmentExpr);
                assn.codeGen(context);
            }
            return context.globals()[id.name];
        }
        else { //local variable declaration
#if DEBUG_GENERATOR
            std::cout << "Creating global variable declaration " << type.name << " " << id.name << endl;
#endif
            AllocaInst *alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
            context.locals()[id.name] = alloc;
            if (assignmentExpr != NULL) {
                    NAssignment assn(id, *assignmentExpr);
                    assn.codeGen(context);
            }
            return alloc;
        }
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context)
{
#if DEBUG_GENERATOR
    std::cout << "clean previous return value" << std::endl;
#endif
    returnValue = NULL;
    vector<Type*> argTypes;
        VariableList::const_iterator it;
        for (it = arguments.begin(); it != arguments.end(); it++) {
                argTypes.push_back(typeOf((**it).type));
        }
        FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
        Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
        BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

        context.pushBlock(bblock);

        Function::arg_iterator argsValues = function->arg_begin();
        Value* argumentValue;
        std::list<Type *> types;

        for (it = arguments.begin(); it != arguments.end(); it++) {
                (**it).codeGen(context);

                argumentValue = argsValues++;
                argumentValue->setName((*it)->id.name.c_str());
                StoreInst *inst = new StoreInst(argumentValue, context.locals()[(*it)->id.name], false, bblock);
                types.push_back(argumentValue->getType());
        }

        Value *blockReturnValue = block.codeGen(context);
        Value *localReturnValue = returnValue != NULL ? returnValue : blockReturnValue;
        ReturnInst::Create(getGlobalContext(), localReturnValue, bblock);

        context.popBlock();
        core::functionList.addFunction(id.name, function, types);
        returnValue = NULL;
#if DEBUG_GENERATOR
        std::cout << "Creating function: " << id.name << endl;
        std::cout << "clean setted return value" << std::endl;
#endif
        return function;
}

