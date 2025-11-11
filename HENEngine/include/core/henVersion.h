#ifndef _HENVERSION_H_
#define _HENVERSION_H_

#include <string>

namespace hen::version
{
    constexpr int MAJOR = 0;
    constexpr int MINOR = 6;
    constexpr int REVISION = 37;

    const std::string VERSION = std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(REVISION);
}

#endif // !_HENVERSION_H_