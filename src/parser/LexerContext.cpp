#include "LexerContext.h"

#include "../utils/config_checked.h"

namespace sugar {
namespace parser {

LexerContext::LexerContext(ProgramNode *programStmtsArg, const std::string *filenameArg,
                           bool interactiveArg, stmtFunction callback): filename(filenameArg)
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

    errorCount = 0;
    hasError = false;
}

LexerContext::~LexerContext(){
}

void LexerContext::onProgramStmt(ast::Node *stmt){
    _callback(programStmts, stmt);
}

} // namespace parser
} // namespace sugar
