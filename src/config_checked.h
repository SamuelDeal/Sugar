#ifndef CONFIG_CHECKED_H
#define CONFIG_CHECKED_H

#include "config.h"

#ifndef INDENT_ALLOW_MIXED
#define INDENT_ALLOW_MIXED 0
#endif

#ifndef DEBUG_LEXER
#define DEBUG_LEXER 0
#endif

#ifndef DEBUG_PARSER
#define DEBUG_PARSER 0
#endif

#ifndef DEBUG_GENERATOR
#define DEBUG_GENERATOR 0
#endif

#ifndef DEBUG_IR
#define DEBUG_IR 0
#endif

#if !defined(INDENT_SPACE_SIZE) || (INDENT_SPACE_SIZE < 1)
#undef INDENT_SPACE_SIZE
#define INDENT_SPACE_SIZE -1
#endif

#ifdef OS_WINDOWS
#undef LIBREADLINE
#endif
#ifndef LIBREADLINE
#define LIBREADLINE 0
#endif

#ifndef LIBEDITLINE
#define LIBEDITLINE 0
#endif

#ifndef SHELL_USE_COLOR
#define SHELL_USE_COLOR 0
#endif

#ifdef OS_WINDOWS
#undef SHELL_USE_COLOR
#define SHELL_USE_COLOR 0
#endif

#endif // CONFIG_CHECKED_H
