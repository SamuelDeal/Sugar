#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstddef>
#include "config_checked.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

#if __cplusplus < 201103L //C++11
namespace std {
    std::string to_string(int i);
    std::string to_string(unsigned int i);
}
#endif

#endif // UTILS_H
