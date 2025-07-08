#ifndef _HENCONSOLE_H_
#define _HENCONSOLE_H_

#include <string>

namespace hen::console
{
    enum class Level
    {
        Default,
        Warning,
        Error,
        ExtremeError // will bring the console to the front, but we dont have console gui yet :(
    };

    void Toggle();
    void SetLocked(bool lock);

    void Post(const char* message, Level level = Level::Default);
    void Post(const std::string& message, Level level = Level::Default);
}

#endif // !_HENCONSOLE_H_