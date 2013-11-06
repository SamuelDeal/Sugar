SOURCES += \
    sugar.cpp \
    test.cpp \
    test.cpp \
    ast/Node.cpp \
    core/Type.cpp \
    core/Scope.cpp \
    ast/Block.cpp \
    ast/Expression.cpp \
    ast/Statement.cpp \
    ast/Constant.cpp \
    ast/Identifier.cpp \
    ast/FunctionCall.cpp \
    ast/ExpressionStmt.cpp \
    ast/ReturnStmt.cpp \
    ast/Assignment.cpp \
    ast/VariableDeclaration.cpp \
    ast/FunctionDeclaration.cpp \
    ast/Comparison.cpp \
    ast/IfExpression.cpp \
    lexer.cpp \
    parser.cpp \
    gen/Compiler.cpp \
    gen/Interpreter.cpp \
    gen/Generator.cpp \
    core/CastGraph.cpp \
    core/Function.cpp \
    codegen.cpp.old \
    complete.c.old \
    EditLineReader.cpp.old \
    functionimpl.cpp.old \
    functionlist.cpp.old \
    corefn.cpp.old \
    sugarc.cpp.old \
    sugari.cpp.old \
    gen/GeneratedCode.cpp \
    gen/Generation.cpp \
    gen/AbstractGenerator.cpp \
    core/Variable.cpp \
    src/ast/VariableDeclaration.cpp \
    src/ast/ReturnStmt.cpp \
    src/ast/Node.cpp \
    src/ast/IfExpression.cpp \
    src/ast/Identifier.cpp \
    src/ast/FunctionDeclaration.cpp \
    src/ast/FunctionCall.cpp \
    src/ast/Constant.cpp \
    src/ast/Comparison.cpp \
    src/ast/Block.cpp \
    src/ast/Assignment.cpp \
    src/core/Variable.cpp \
    src/core/Type.cpp \
    src/core/Scope.cpp \
    src/core/Function.cpp \
    src/core/CastGraph.cpp \
    src/gen/Interpreter.cpp \
    src/gen/Generator.cpp \
    src/gen/Generation.cpp \
    src/gen/GeneratedCode.cpp \
    src/gen/Compiler.cpp \
    src/sugar.cpp \
    src/core/AbstractFunction.cpp \
    src/core/Operator.cpp \
    src/core/Cast.cpp \
    src/ast/Operator.cpp \
    src/ast/TypeIdentifier.cpp \
    src/sugari.cpp \
    src/utils.cpp \
    src/sugarc.cpp \
    src/ast/WhileStmt.cpp \
    src/gen/Parser.cpp \
    src/gen/InteractiveParser.cpp \
    src/parser/BatchParser.cpp \
    src/parser/InteractiveParser.cpp \
    src/parser/LexerContext.cpp \
    src/gen/pass/AbstractPass.cpp \
    src/types/Int.cpp \
    src/types/BuiltinType.cpp \
    src/types/Float.cpp \
    src/types/Bool.cpp \
    src/gen/pass/IrPass.cpp \
    src/ast/ArgumentDeclaration.cpp \
    src/gen/pass/FunctionLookupPass.cpp \
    src/ast/FunctionImplementation.cpp \
    src/core/LanguageError.cpp \
    src/ast/NodeData.cpp

HEADERS += \
    codegen.h \
    functionlist.h \
    functionimpl.h \
    complete.h \
    EditLineReader.h \
    LineReader.h \
    config.h \
    ast/Node.h \
    core/Type.h \
    core/Scope.h \
    ast/Block.h \
    ast/Expression.h \
    ast/Statement.h \
    ast/Constant.h \
    ast/Identifier.h \
    ast/FunctionCall.h \
    ast/ExpressionStmt.h \
    ast/ReturnStmt.h \
    ast/Assignment.h \
    ast/VariableDeclaration.h \
    ast/FunctionDeclaration.h \
    ast/Comparison.h \
    ast/IfExpression.h \
    ast/all.h \
    lexer.hpp \
    node2.h \
    gen/Compiler.h \
    gen/Interpreter.h \
    gen/Generator.h \
    core/CastGraph.h \
    core/Function.h \
    gen/GeneratedCode.h \
    gen/Generation.h \
    gen/AbstractGenerator.h \
    core/Variable.h \
    src/ast/VariableDeclaration.h \
    src/ast/ReturnStmt.h \
    src/ast/Node.h \
    src/ast/IfExpression.h \
    src/ast/Identifier.h \
    src/ast/FunctionDeclaration.h \
    src/ast/FunctionCall.h \
    src/ast/Constant.h \
    src/ast/Comparison.h \
    src/ast/Block.h \
    src/ast/Assignment.h \
    src/ast/all.h \
    src/core/Variable.h \
    src/core/Type.h \
    src/core/Scope.h \
    src/core/Function.h \
    src/core/CastGraph.h \
    src/gen/Generator.h \
    src/gen/Generation.h \
    src/gen/GeneratedCode.h \
    src/gen/Compiler.h \
    src/gen/Interpreter.h \
    src/core/AbstractFunction.h \
    src/core/Operator.h \
    src/core/Cast.h \
    src/ast/Operator.h \
    src/ast/TypeIdentifier.h \
    src/config.h \
    src/utils.h \
    src/lexer_ctx.h \
    src/ast/WhileStmt.h \
    src/gen/Parser.h \
    src/gen/InteractiveParser.h \
    src/parser/BatchParser.h \
    src/parser/InteractiveParser.h \
    src/parser/LexerContext.h \
    src/config_checked.h \
    src/gen/pass/AbstractPass.h \
    src/types/all.h \
    src/types/Int.h \
    src/types/BuiltinType.h \
    src/types/Float.h \
    src/types/Bool.h \
    src/gen/pass/IrPass.h \
    src/ast/ArgumentDeclaration.h \
    src/gen/pass/FunctionLookupPass.h \
    src/ast/FunctionImplementation.h \
    src/core/LanguageError.h \
    src/ast/NodeData.h \
    src/utils/Getter.h \
    src/utils/Getter.tpl.h

OTHER_FILES += \
    parser.y \
    lexer.l \
    CMakeLists.txt \
    example.txt \
    Makefile \
    example2.txt \
    README.md \
    src/lexer.l \
    src/parser.y \
    test/test9.test \
    test/test9.expected \
    test/test8.test \
    test/test8.expected \
    test/test7.test \
    test/test7.expected \
    test/test6.test \
    test/test6.expected \
    test/test5.test \
    test/test5.expected \
    test/test4.test \
    test/test4.expected \
    test/test3.test \
    test/test3.expected \
    test/test2.test \
    test/test2.expected \
    test/test1.test \
    test/test010.test \
    test/test010.expected \
    test/test009.test \
    test/test009.expected \
    test/test008.test \
    test/test008.expected \
    test/test007.test \
    test/test007.expected \
    test/test006.test \
    test/test006.expected \
    test/test005.test \
    test/test005.expected \
    test/test004.test \
    test/test004.expected \
    test/test003.test \
    test/test003.expected \
    test/test002.test \
    test/test002.expected \
    test/test001.test \
    test/test001.expected \
    src/parser/parser.y \
    src/parser/lexer.l \
    test/test022.test \
    test/test022.expected \
    test/test021.test \
    test/test021.expected \
    test/test020.test \
    test/test020.expected \
    test/test019.test \
    test/test025.test \
    test/test025.expected \
    test/test024.test \
    test/test024.expected \
    test/test023.test \
    test/test023.expected \
    test/test019.expected \
    test/test018.test \
    test/test018.expected \
    test/test017.test \
    test/test017.expected \
    test/test016.test \
    test/test016.expected \
    test/test015.test \
    test/test015.expected \
    test/test014.test \
    test/test014.expected \
    test/test013.test \
    test/test013.expected \
    test/test012.test \
    test/test012.expected \
    test/test011.test \
    test/test011.expected
