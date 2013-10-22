#ifndef LEXER_CTX_H
#define LEXER_CTX_H

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

struct LexerContext {
    IndentInfo indentContext;
    unsigned int lineIndents;
    unsigned int commentStartCount;
    std::queue<int> tokens;
    std::queue<int> latentTokens;
    std::stack<IndentContext*> indents;
    bool pendingEndInstr;
    bool eof;
};



#endif // LEXER_CTX_H
