#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstddef>
#include "config.h"

#if __cplusplus < 201103L //C++11
namespace std {
    std::string to_string(int i);
}
#endif

#endif // UTILS_H
