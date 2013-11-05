#ifndef SUGAR_UTILS_GETTER_H
#define SUGAR_UTILS_GETTER_H

#include <functional>

namespace sugar {
namespace utils {


template<class A>
class Getter {
public:
    Getter();
    Getter(std::function<A ()> generator);
    Getter(const A& value);
    ~Getter();
    A get();
    operator A();
    bool isValid() const;

protected:
    bool _generated;
    bool _valid;
    A _value;
    std::function<A ()> _generator;
};

} // namespace utils
} // namespace sugar

#endif // SUGAR_UTILS_GETTER_H
