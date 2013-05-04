#ifndef SUGAR_CORE_CASTGRAPH_H
#define SUGAR_CORE_CASTGRAPH_H

#include <list>
#include <map>

namespace sugar {
namespace core {

class Type;
class Cast;
class Function;

class CastGraph
{
public:
    CastGraph();
    ~CastGraph();

    void addImplicitCast(Cast *cast);
    std::list<Cast *> getImplicitCastPath(const Type *from, const Type *to) const;
    std::list<const Type *> getImplicitCastableTo(const Type *from) const;

protected:
    std::list<Cast*> _implicitCasts;
    std::multimap<const Type*, Cast*> _implicitFroms;
    bool _findResults(std::list<Cast *> &result, const Type *from, const Type *to) const;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_CASTGRAPH_H
