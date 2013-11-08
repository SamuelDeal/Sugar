#ifndef SUGAR_PARSER_LEXERCONTEXT_H
#define SUGAR_PARSER_LEXERCONTEXT_H

#include <cstddef>
#include <queue>
#include <stdarg.h>
#include <stack>
#include <string>

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
    class Node;
}

namespace parser {

class ProgramNode;

typedef void (*stmtFunction)(ProgramNode *programStmts, ast::Node *stmt);


class LexerContext
{
public:
    LexerContext(ProgramNode *programStmts, const std::string *filename, bool interactive, stmtFunction callback);
    ~LexerContext();
    void onProgramStmt(ast::Node *stmt);

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
    ProgramNode *programStmts;
    const std::string *filename;
    unsigned int errorCount;
    bool hasError;

protected:
    stmtFunction _callback;
};

} // namespace parser
} // namespace sugar

#endif // SUGAR_PARSER_LEXERCONTEXT_H
