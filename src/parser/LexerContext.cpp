#include "LexerContext.h"

#include "../config_checked.h"

namespace sugar {
namespace parser {

LexerContext::LexerContext(ast::Node *programStmtsArg,
                            bool interactiveArg, stmtFunction callback)
{
    interactive = interactiveArg;
    programStmts = programStmtsArg;
    _callback = callback;

    eof = false;
    pendingEndInstr = false;
    lineIndents = 0;

    indentContext.style = IndentInfo::UNDEFINED;
    indentContext.spaceSize = INDENT_SPACE_SIZE;

    indents.push(new IndentContext());
    indents.top()->start = 0;
    indents.top()->current = 0;
}

void LexerContext::onProgramStmt(ast::Node *stmt){
    _callback(programStmts, stmt);
}

} // namespace parser
} // namespace sugar
