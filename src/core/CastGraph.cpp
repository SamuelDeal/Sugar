#include "CastGraph.h"

#include "Cast.h"

namespace sugar {
namespace core {

CastGraph::CastGraph()
{
}

CastGraph::~CastGraph(){
    for(std::list<Cast *>::iterator i = _implicitCasts.begin(); i != _implicitCasts.end(); i++){
        delete (*i);
    }
}

void CastGraph::addImplicitCast(Cast *cast){
    _implicitCasts.push_back(cast);
    _implicitFroms.insert(std::pair<const Type*, Cast*>(cast->getFrom(), cast));
}

std::list<const Type *> CastGraph::getImplicitCastableTo(const Type *from) const {
    typedef std::multimap<const Type*,Cast*>::const_iterator CastIt;
    std::list<const Type *> result;
    std::pair <CastIt, CastIt> fromCasts;
    fromCasts = _implicitFroms.equal_range(from);
    for(CastIt it=fromCasts.first; it!=fromCasts.second; ++it){
        result.push_back(it->second->getTo());
    }
    return result;
}

std::list<Cast *> CastGraph::getImplicitCastPath(const Type *from, const Type *to) const {
    std::list<Cast *> path;
    std::list<Cast *> result;
    if(_findResults(path, from, to)){
        result.insert(result.end(), path.rbegin(), path.rend());
    }
    return result;
}

bool CastGraph::_findResults(std::list<Cast *> &result, const Type *from, const Type *to) const {
    typedef std::multimap<const Type*,Cast*>::const_iterator CastIt;

    std::pair <CastIt, CastIt> fromCasts;
    fromCasts = _implicitFroms.equal_range(from);
    for(CastIt it=fromCasts.first; it!=fromCasts.second; ++it){
        if(it->second->getTo() == to){
            result.push_back(it->second);
            return true;
        }
        else {
            if(_findResults(result, it->second->getTo(), to)){
                result.push_back(it->second);
                return true;
            }
        }
    }
    return false;
}

} // namespace core
} // namespace sugar

