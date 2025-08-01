#include "tools/henConsole.h"

#include <iostream>
#include <fstream>
#include <ctime>

#define TIME_FORMAT "%d/%m/%Y %H:%M:%S"

namespace hen::console
{
    bool Initialised = false;

    std::string LogFileName = "console.txt";
    std::filesystem::path LogFileDir = "./";
    std::filesystem::path LogFilePath = LogFileDir / LogFileName;
    std::ofstream LogFile{};

    void Initialise()
    {
        if(!std::filesystem::is_directory(LogFileDir))
        {
            bool result = std::filesystem::create_directory(LogFileDir);
            HEN_ASSERT(result, "[hen::console] Couldn't create log dir");
            Initialised = false;
        }

        LogFile = std::ofstream(LogFilePath, std::ios::out); // overwrite any existing file
        HEN_ASSERT(LogFile.is_open(), "[hen::console] Failed to open log file");

        LogFile << R"(
  _____ ____  _   _  _____  ____  _      ______       _________   _________ 
 / ____/ __ \| \ | |/ ____|/ __ \| |    |  ____|     |__   __\ \ / /__   __|
| |   | |  | |  \| | (___ | |  | | |    | |__           | |   \ V /   | |   
| |   | |  | | . ` |\___ \| |  | | |    |  __|          | |    > <    | |   
| |___| |__| | |\  |____) | |__| | |____| |____   _     | |   / . \   | |   
 \_____\____/|_| \_|_____/ \____/|______|______| (_)    |_|  /_/ \_\  |_|   
            )" << "\n";

        auto timeNow = std::time(nullptr);
        auto time = *std::localtime(&timeNow);

        LogFile << "[hen::console] Log file created - " << std::put_time(&time, TIME_FORMAT) << "\n";

        Initialised = true;
        HEN_ASSERT(Initialised, "[hen::console] Failed to initialise");
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
        case LOGLEVEL::ASSERTION:
            levelText = "ASSERTION FAILED: ";
            textColour = "\x1b[31m";
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