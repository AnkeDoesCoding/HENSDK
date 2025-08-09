#include "tools/henConsole.h"

#include <iostream>
#include <fstream>
#include <ctime>

#define TIME_FORMAT "%d/%m/%Y %H:%M:%S"

namespace hen::console
{
    bool Initialised = false;

    static std::string LogFileName = "console.txt";
    static std::filesystem::path LogFileDir = "./";
    static std::filesystem::path LogFilePath = LogFileDir / LogFileName;
    static std::ofstream LogFile{};

    void Initialise()
    {
        if(!std::filesystem::is_directory(LogFileDir))
        {
            bool result = std::filesystem::create_directory(LogFileDir);
            Initialised = result;
            HEN_ASSERT(result, "[hen::console] Couldn't create log dir");
        }

        LogFile = std::ofstream(LogFilePath, std::ios::out); // overwrite any existing file
        HEN_ASSERT(LogFile.is_open(), "[hen::console] Failed to open log file");

        auto timeNow = std::time(nullptr);
        auto time = *std::localtime(&timeNow);

        LogFile << "[hen::console] Log file opened - " << std::put_time(&time, TIME_FORMAT) << "\n";

        Initialised = true;
    }

    void Shutdown()
    {
        auto timeNow = std::time(nullptr);
        auto time = *std::localtime(&timeNow);

        LogFile << "[hen::console] Closing log file - " << std::put_time(&time, TIME_FORMAT);

        if(LogFile.is_open())
        {
            LogFile.close();
        }
    }

    void Post(const std::string &message, LOGLEVEL level) 
    {
        std::string levelText;
        std::string textColour;
    
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

        auto timeNow = std::time(nullptr);
        auto time = *std::localtime(&timeNow);

        LogFile << levelText << message << " - " << std::put_time(&time, TIME_FORMAT) << "\n";

        #if DEBUG
            std::cout << textColour << levelText << message  << " - " << std::put_time(&time, TIME_FORMAT) << "\033[0m\n" <<  std::flush;
        #endif // !DEBUG
    }
}