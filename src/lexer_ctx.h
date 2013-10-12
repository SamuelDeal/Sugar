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

struct LexerContext {
    IndentInfo indentContext;
    unsigned int lineIndents;
    std::queue<int> tokens;
    std::queue<int> latentTokens;
    std::stack<int> indents;
    bool eof;
};

void lexStartBlock();

#endif // LEXER_CTX_H
