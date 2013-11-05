#ifndef SUGAR_CORE_LANGUAGEERROR_H
#define SUGAR_CORE_LANGUAGEERROR_H

#include <string>

namespace sugar {
namespace core {

class LanguageError
{
public:
    LanguageError(const std::string &errorMsg);

    LanguageError* getLinked() const;
    void setLinked(LanguageError* );
    const std::string& getMessage();

protected:
    LanguageError *_linked;
    std::string _message;
};

} // namespace core
} // namespace sugar

#endif // SUGAR_CORE_LANGUAGEERROR_H
