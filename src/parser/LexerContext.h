#ifndef SUGAR_PARSER_LEXERCONTEXT_H
#define SUGAR_PARSER_LEXERCONTEXT_H

#include <cstddef>
#include <queue>
#include <stdarg.h>
#include <stack>

struct IndentInfo {
    enum EStyle {
        UNDEFINED = 0,
        SPACE = 1,
        TAB = 2
    };

    int spaceSize;
    EStyle style;
};

struct IndentContext {
    int current;
    int start;
};

namespace sugar {

namespace ast {
    class Block;
    class Statement;
}

namespace parser {

typedef void (*stmtFunction)(ast::Block *programStmts, ast::Statement *stmt);


class LexerContext
{
public:
    LexerContext(ast::Block *programStmts, bool interactive, stmtFunction callback);
    void onProgramStmt(sugar::ast::Statement *stmt);

    IndentInfo indentContext;
    unsigned int lineIndents;
    unsigned int commentStartCount;
    std::queue<int> tokens;
    std::queue<int> latentTokens;
    std::stack<IndentContext*> indents;
    bool pendingEndInstr;
    bool eof;
    void *scanner;
    bool interactive;
    ast::Block *programStmts;

protected:
    stmtFunction _callback;
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_LEXERCONTEXT_H
