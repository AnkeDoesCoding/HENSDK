#include "tools/henConsole.h"

#include "vendor/imgui/imgui.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <deque>
#include <vector>
#include <sstream>
#include <mutex>

#define TIME_FORMAT "%H:%M:%S"
#define MAX_DISPLAYED_LOGS 20000

namespace hen::console
{
    bool Initialised = false;
    bool Visible = false;
    bool Locked = false;

    static std::string LogFileName = "console.txt";
    static std::filesystem::path LogFileDir = "./";
    static std::filesystem::path LogFilePath = LogFileDir / LogFileName;
    static std::ofstream LogFile{};

    struct LogEntry
    {
        std::string Message;
        std::string Time;
        LOGLEVEL Level;
    };

    static std::deque<LogEntry> Entries;
    static std::mutex Mutex; // yeah bitch this shi gonna be thread safe, if i managed to do it right
    static char InputBuffer[512] = "";
    static std::vector<std::string> CommandHistory;
    static int HistoryIndex = -1;

    static std::string GetCurrentTime()
    {
        auto now = std::time(nullptr);
        auto time = *std::localtime(&now);
        std::ostringstream timeSString;
        timeSString << std::put_time(&time, TIME_FORMAT);
        return timeSString.str();
    }

    static int InputCallback(ImGuiInputTextCallbackData* data)
    {
        if (data->EventChar == '~' || data->EventChar == '`')
        {
            return 1;
        }

        if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
        {
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (HistoryIndex == -1 && !CommandHistory.empty())
                {
                    HistoryIndex = (int)CommandHistory.size() - 1;
                }
                else if (HistoryIndex > 0)
                {
                    HistoryIndex--;
                }

                if (HistoryIndex >= 0 && HistoryIndex < (int)CommandHistory.size())
                {
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, CommandHistory[HistoryIndex].c_str());
                }
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (HistoryIndex != -1)
                {
                    HistoryIndex++;
                    if (HistoryIndex >= (int)CommandHistory.size())
                    {
                        HistoryIndex = -1;
                        data->DeleteChars(0, data->BufTextLen);
                    }
                    else
                    {
                        data->DeleteChars(0, data->BufTextLen);
                        data->InsertChars(0, CommandHistory[HistoryIndex].c_str());
                    }
                }
            }
        }
        return 0;
    }

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

        Initialised = true;
    }

    void Shutdown()
    {
        if(LogFile.is_open())
        {
            LogFile.close();
        }
    }

    void Draw()
    {
        if(!Visible || Locked)
        {
            return;
        }

        ImGui::Begin("Console", &Visible, ImGuiWindowFlags_None);

        float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

        ImGui::BeginChild("Scrolling Region", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
        for(auto &entry : Entries)
        {
            ImVec4 color;
            switch(entry.Level)
            {
            case LOGLEVEL::INFO:
                color = ImVec4(1,1,1,1);
                break;
            case LOGLEVEL::WARNING:
                color = ImVec4(1,1,0,1);
                break;
            case LOGLEVEL::ERROR:
                color = ImVec4(1,0,0,1);
                break;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("%s - %s", entry.Message.c_str(), entry.Time.c_str());
            ImGui::PopStyleColor();
        }

        if(!Locked && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
        ImGui::Separator();
        ImGui::PushItemWidth(-1);

        if (ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere();
        }

        if(ImGui::InputText(" ", InputBuffer, IM_ARRAYSIZE(InputBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackCharFilter, InputCallback))
        {
            if(InputBuffer[0] != '\0')
            {
                Log(std::string("[User] ") + InputBuffer, LOGLEVEL::INFO);

                if (CommandHistory.empty() || CommandHistory.back() != InputBuffer)
                {
                    CommandHistory.push_back(InputBuffer);
                }
                HistoryIndex = -1;

                //commands and future cvar shit

                InputBuffer[0] = '\0';

                ImGui::SetKeyboardFocusHere(-1);
            }
        }


        ImGui::PopItemWidth();
        ImGui::End();
    }

    void Toggle()
    {
        Visible = !Visible;
    }

    void SetLocked(bool lock)
    {
        Locked = lock;
    }

    void Log(const std::string &message, LOGLEVEL level) 
    {
        std::lock_guard<std::mutex> lock(Mutex);
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

        LogEntry entry
        {
            message,
            GetCurrentTime(),
            level
        };

        Entries.push_back(entry);   

        if(Entries.size() > MAX_DISPLAYED_LOGS)
        {
            Entries.pop_front();
        }

        LogFile << levelText << message << " - " << GetCurrentTime() << "\n";
        

        #if DEBUG
            std::cout << textColour << levelText << message  << " - " << GetCurrentTime() << "\033[0m\n" <<  std::flush;
        #endif // !DEBUG
    }
}