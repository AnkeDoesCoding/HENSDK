#include "tools/henConsole.h"

#include <iostream>

namespace hen
{
    void console::Post(const char* message, LOGLEVEL level)
    {
        #if DEBUG
            std::string levelText;
            const char* textColour = nullptr;

            switch (level)
            {
            case LOGLEVEL::INFO:
                levelText = "";
                textColour = "\x1b[37m";
                break;
            case LOGLEVEL::WARNING:
                levelText = "WARNING: ";
                textColour = "\x1b[33m";
                break;
            case LOGLEVEL::ERROR:
                levelText = "ERROR: ";
                textColour = "\x1b[31m";
                break;
            default:
                break;
            }
        
            std::cout << textColour + levelText + message + "\033[0m" << std::endl;
        #endif // !DEBUG
    }

    void console::Post(const std::string& message, LOGLEVEL level)
    {
        #if DEBUG
            std::string levelText;
            const char* textColour = nullptr;
        
            switch (level)
            {
            case LOGLEVEL::INFO:
                levelText = "";
                textColour = "\x1b[37m";
                break;
            case LOGLEVEL::WARNING:
                levelText = "WARNING: ";
                textColour = "\x1b[33m";
                break;
            case LOGLEVEL::ERROR:
                levelText = "ERROR: ";
                textColour = "\x1b[31m";
                break;
            default:
                break;
            }
        
            std::cout << textColour + levelText + message + "\033[0m" << std::endl;
        #endif // !DEBUG
    }
}