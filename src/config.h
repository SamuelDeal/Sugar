#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG_LEXER 0
#define DEBUG_PARSER 0
#define DEBUG_GENERATOR 0
#define DEBUG_IR 0

#define INDENT_ALLOW_MIXED 0 //source indentation could mix tabs and spaces ?
#define INDENT_FAIL_ON_ERROR 0 //lexer fail on error or emit a BAD_INDENT_TOKEN ?

#define LIBREADLINE 1
#define LIBEDITLINE 0

#endif // CONFIG_H
