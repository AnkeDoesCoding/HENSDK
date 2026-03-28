#ifndef _HENVERSION_H_
#define _HENVERSION_H_

#include <string>

namespace hen::version
{
    constexpr int Major = 0;
    constexpr int Minor = 11;
    constexpr int Revision = 74;

    const std::string Version = std::to_string(Major) + "." + std::to_string(Minor) + "." + std::to_string(Revision);
}

#endif // !_HENVERSION_H_