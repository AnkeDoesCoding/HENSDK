#ifndef _HENCONSOLE_H_
#define _HENCONSOLE_H_

#include <string>

namespace hen::console
{
    enum class LOGLEVEL
    {
        INFO,
        WARNING,
        ERROR
    };

    void Toggle();
    void SetLocked(bool lock);

    void Post(const char* message, LOGLEVEL level = LOGLEVEL::INFO);
    void Post(const std::string& message, LOGLEVEL level = LOGLEVEL::INFO);
}

#endif // !_HENCONSOLE_H_