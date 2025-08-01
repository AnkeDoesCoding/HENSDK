#ifndef _HENCONSOLE_H_
#define _HENCONSOLE_H_

#include <string>
#include <cassert>

#ifdef DEBUG
    #define HEN_CONSOLE_ASSERT(cond, msg) ((cond) ? (void)0 : (hen::console::Post(msg, hen::console::LOGLEVEL::ERROR), assert((cond) && msg))) // Posts message to console and (if debug configuration) throws an assertion
#else
    #define HEN_CONSOLE_ASSERT(cond, msg) (hen::console::Post(msg, hen::console::LOGLEVEL::ERRROR))
#endif // !DEBUG

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