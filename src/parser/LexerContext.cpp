#include "LexerContext.h"

#include "../config.h"

#include "../ast/Statement.h"
#include "../ast/Block.h"

namespace sugar {
namespace parser {

LexerContext::LexerContext(ast::Block *programStmts,
                            bool interactiveArg, stmtFunction callback)
{
    interactive = interactiveArg;
    _programStmts = programStmts;
    _callback = callback;

    eof = false;
    pendingEndInstr = false;
    lineIndents = 0;

    indentContext.spaceSize = -1;
    indentContext.style = IndentInfo::UNDEFINED;

    indents.push(new IndentContext());
    indents.top()->start = 0;
    indents.top()->current = 0;
#ifdef INDENT_SPACE_SIZE
    indentContext.spaceSize = INDENT_SPACE_SIZE;
#endif
}

void LexerContext::onProgramStmt(sugar::ast::Statement *stmt){
    _programStmts->stmts.push_back(stmt);
    if(_callback != NULL){
        _callback(stmt);
    }
}

} // namespace parser
} // namespace sugar
