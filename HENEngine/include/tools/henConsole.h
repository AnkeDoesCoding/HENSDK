#ifndef _HENCONSOLE_H_
#define _HENCONSOLE_H_

#include <string>
#include <cassert>

#if DEBUG
    #if PLATFORM_WINDOWS
        #define HEN_DEBUG_BREAK() __debugbreak()
    #elif PLATFORM_LINUX
        #define HEN_DEBUG_BREAK() __builtin_debugtrap()
    #endif // !PLATFORM_WINDOWS
#else
    #define HEN_DEBUG_BREAK() ((void)0)
#endif // !DEBUG

#define HEN_ASSERT(cond, msg) ((cond) ? (void) 0 : (hen::console::Post(msg, hen::console::LOGLEVEL::ERROR), assert(cond))) // Posts message to console and if debug configuration, calls assert

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