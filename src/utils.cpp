#include "utils.h"

#include <sstream>

#if __cplusplus < 201103L //C++11
namespace std {

std::string to_string(int i){
    std::stringstream out;
    out << i;
    return out.str();
}

}
#endif
