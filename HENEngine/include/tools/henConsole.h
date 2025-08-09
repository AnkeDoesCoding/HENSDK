#ifndef _HENCONSOLE_H_
#define _HENCONSOLE_H_

#include <string>
#include <filesystem>
#include <cassert>

#if DEBUG
    #if PLATFORM_WINDOWS
        #define HEN_DEBUG_BREAK() __debugbreak();
    #elif PLATFORM_LINUX
        #define HEN_DEBUG_BREAK() __builtin_debugtrap();
    #endif // !PLATFORM_WINDOWS
#else
    #define HEN_DEBUG_BREAK() ((void)0)
#endif // !DEBUG

#define HEN_ASSERT(cond, msg) ((cond) ? (void) 0 : (hen::console::Post(std::string(msg) + " [ FILE: " + __FILE__ + " LINE: " + std::to_string(__LINE__) + " ]", hen::console::LOGLEVEL::ERROR), hen::console::Shutdown(), assert(cond)))
namespace hen::console
{
    enum class LOGLEVEL
    {
        INFO,
        WARNING,
        ERROR
    };

    void Initialise();
    void Shutdown();

    void Toggle();
    void SetLocked(bool lock);

    void Post(const std::string& message, LOGLEVEL level = LOGLEVEL::INFO);

    extern bool Initialised;
}

#endif // !_HENCONSOLE_H_