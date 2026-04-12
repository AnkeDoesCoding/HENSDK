#ifndef _HENVERSION_H_
#define _HENVERSION_H_

#include <format>

namespace hen::version
{
    constexpr int Major = 0;
    constexpr int Minor = 11;
    constexpr int Revision = 81;

    inline std::string Version = std::format("{}.{}.{}", Major, Minor, Revision);
}

#endif // !_HENVERSION_H_