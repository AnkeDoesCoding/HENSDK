#include "tools/henConsole.h"

#include <iostream>

namespace hen
{
    void console::Post(const char* message, Level level)
    {
#if DEBUG
        std::string levelText;

        switch (level)
        {
        case Level::Default:
            levelText = "";
            break;
        case Level::Warning:
            levelText = "WARNING: ";
            break;
        case Level::Error:
            levelText = "ERROR: ";
            break;
        case Level::ExtremeError:
            levelText = "EXTREME ERROR:";
            break;
        default:
            break;
        }

        std::cout << "" + levelText + message << std::endl;
#endif // !DEBUG
    }

    void console::Post(const std::string& message, Level level)
    {
#if DEBUG
        std::string levelText;

        switch (level)
        {
        case Level::Default:
            levelText = "";
            break;
        case Level::Warning:
            levelText = "WARNING: ";
            break;
        case Level::Error:
            levelText = "ERROR: ";
            break;
        case Level::ExtremeError:
            levelText = "EXTREME ERROR: ";
            break;
        default:
            break;
        }

        std::cout << "" + levelText + message << std::endl;
#endif // !DEBUG
    }
}