#ifndef _HENCONSOLE_H_
#define _HENCONSOLE_H_

#include <string>
#include <filesystem>
#include <cassert>

#define HEN_LOG(msg) hen::console::Log(msg, hen::console::LOG_LEVELS::INFO)
#define HEN_WARN(msg) hen::console::Log(msg, hen::console::LOG_LEVELS::WARNING)
#define HEN_ERROR(msg) hen::console::Log(msg, hen::console::LOG_LEVELS::ERROR)

#if DEBUG
    #if PLATFORM_WINDOWS
        #define HEN_DEBUG_BREAK() __debugbreak();
    #elif PLATFORM_LINUX
        #define HEN_DEBUG_BREAK() __builtin_debugtrap();
    #endif // !PLATFORM_WINDOWS
#else
    #define HEN_NO_DEBUGBREAK
#endif // !DEBUG

#ifdef HEN_NO_ASSERT
    #define HEN_ASSERT(cond, msg) ((void)0)
#else
    #define HEN_ASSERT(cond, msg) ((cond) ? (void) 0 : (hen::console::Log(std::string(msg) + " ( FILE: " + __FILE__ + " || LINE: " + std::to_string(__LINE__) + " )", hen::console::LOG_LEVELS::ASSERT), hen::console::Shutdown(), assert(cond)))
#endif // !HEN_NO_ASSERT

#ifdef HEN_NO_DEBUGBREAK
    #define HEN_DEBUG_BREAK() ((void)0)
#endif // !HEN_NO_DEBUGBREAK


namespace hen::console
{
    enum class LOG_LEVELS
    {
        INFO,
        WARNING,
        ERROR,
        ASSERT
    };

    void Initialise();
    void Shutdown();

    void Toggle();
    void SetLocked(bool lock);

    void Log(const std::string& message, LOG_LEVELS level = LOG_LEVELS::INFO);

    extern bool Initialised;
    extern bool Visible;
    extern bool Locked;
}

#endif // !_HENCONSOLE_H_