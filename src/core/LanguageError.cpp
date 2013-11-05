#include "LanguageError.h"

#include <cstddef>

namespace sugar {
namespace core {

LanguageError::LanguageError(const std::string &errorMsg) {
    _message = errorMsg;
    _linked = NULL;
}

LanguageError* LanguageError::getLinked() const{
    return _linked;
}

void LanguageError::setLinked(LanguageError *newError) {
    if(_linked == NULL){
        _linked = newError;
        return;
    }

    LanguageError *last = _linked;
    while(last->_linked != NULL){
        last = last->_linked;
    }
    last->_linked = newError;
}

const std::string& LanguageError::getMessage() {
    return _message;
}

} // namespace core
} // namespace sugar
