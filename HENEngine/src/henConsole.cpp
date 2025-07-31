#include "tools/henConsole.h"

#include <iostream>

namespace hen
{
    void console::Post(const char* message, LOGLEVEL level)
    {
        #if DEBUG
            std::string levelText;
        
            switch (level)
            {
            case LOGLEVEL::INFO:
                levelText = "";
                break;
            case LOGLEVEL::WARNING:
                levelText = "WARNING: ";
                break;
            case LOGLEVEL::ERROR:
                levelText = "ERROR: ";
                break;
            default:
                break;
            }
        
            std::cout << "" + levelText + message << std::endl;
        #endif // !DEBUG
    }

    void console::Post(const std::string& message, LOGLEVEL level)
    {
        #if DEBUG
            std::string levelText;
        
            switch (level)
            {
            case LOGLEVEL::INFO:
                levelText = "";
                break;
            case LOGLEVEL::WARNING:
                levelText = "WARNING: ";
                break;
            case LOGLEVEL::ERROR:
                levelText = "ERROR: ";
                break;
            default:
                break;
            }
        
            std::cout << "" + levelText + message << std::endl;
        #endif // !DEBUG
    }
}