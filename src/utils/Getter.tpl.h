#ifndef SUGAR_UTILS_GETTER_TPL_H
#define SUGAR_UTILS_GETTER_TPL_H

#include "Getter.h"
#include <exception>

namespace sugar {
namespace utils {

template<class A>
Getter<A>::Getter(){
    _valid = false;
}

template<class A>
Getter<A>::Getter(std::function<A ()> generator){
    _generator = generator;
    _generated = false;
    _valid = true;
}

template<class A>
Getter<A>::Getter(const A& value){
    _value = value;
    _generated = true;
    _valid = true;
}

template<class A>
Getter<A>::~Getter(){
}

template<class A>
A Getter<A>::get(){
    if(!_valid){
        throw new std::exception();
    }
    if(!_generated){
        _value = _generator();
        _generated = true;
    }
    return _value;
}

template<class A>
Getter<A>::operator A(){
    return get();
}

template<class A>
bool Getter<A>::isValid() const {
    return _valid;
}

} // namespace utils
} // namespace sugar

#endif // SUGAR_UTILS_GETTER_TPL_H
