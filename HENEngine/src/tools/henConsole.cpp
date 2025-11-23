#include "tools/henConsole.h"

#include "core/henCVar.h"
#include "ui/henUI.h"

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

    struct LogEntry
    {
        std::string Message;
        std::string Time;
        LOGLEVEL Level;
    };

    static std::ofstream LogFile{};
    static std::deque<LogEntry> Entries;

    static std::mutex Mutex; // yeah bitch this shi gonna be thread safe, if i managed to do it right

    static char InputBuffer[1024] = "";

    static std::vector<std::string> CommandHistory;
    static std::vector<std::string> AutocompleteMatches;
    static std::vector<std::string> AutocompleteDisplay;
    static int HistoryIndex = -1;
    static int AutoIndex = -1;

    static std::string GetCurrentTime()
    {
        auto now = std::time(nullptr);
        auto time = *std::localtime(&now);
        std::ostringstream timeSString;
        timeSString << std::put_time(&time, TIME_FORMAT);
        return timeSString.str();
    }

    static bool ParseBool(const std::string& inputString)
    {
        if (inputString == "1" || inputString == "true" )
        {
            return true;
        }
        if (inputString == "0" || inputString == "false")
        {
            return false;
        }

        console::Log("[hen::console] Invalid bool string: " + inputString, console::LOGLEVEL::WARNING);
        return false;
    }

    static void DisplayValue(cvar::CVar* cvar)
    {
        std::visit([&](auto&& val)
        {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>)
            {
                console::Log("[hen::console] " + cvar->Name + " = " + val);
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                console::Log("[hen::console] " + cvar->Name + " = " + std::string(val ? "true" : "false"));
            }
            else
            {
                console::Log("[hen::console] " + cvar->Name + " = " + std::to_string(val));
            }
        }, cvar->Value);
    }

    void Execute(const std::string& command)
    {
        std::istringstream iSString(command);
        std::vector<std::string> tokens;
        std::string token;

        while (iSString >> token)
        {
            tokens.push_back(token);
        }
        
        if (tokens.empty())
        {
            return;
        }

        std::string cvarName = tokens[0];

        if (auto* cvar = cvar::GetSystem()->GetCVar(cvarName))
        {
            if (tokens.size() == 1)
            {
                DisplayValue(cvar);
            }
            else
            {
                if ((cvar->Flag & cvar::FLAGS_PROTECTED) && cvar::cvar_ProtectionEnabled.GetBool())
                {
                    console::Log("[hen::console] Protection is enabled, type protection_enabled 0 to disable it", console::LOGLEVEL::WARNING);
                    return;
                }

                try 
                {
                    if (std::holds_alternative<int>(cvar->Value))
                    {
                        cvar->Set(std::stoi(tokens[1]));
                    }
                    else if (std::holds_alternative<float>(cvar->Value))
                    {
                        cvar->Set(std::stof(tokens[1]));
                    }
                    else if (std::holds_alternative<bool>(cvar->Value))
                    {
                        cvar->Set(ParseBool(tokens[1]));
                    }
                    else
                    {
                        std::string newValue;
                        for (size_t i = 1; i < tokens.size(); ++i)
                        {
                            if (i > 1) newValue += " ";
                            newValue += tokens[i];
                        }
                        cvar->Set(newValue);
                    }
                }
                catch (...) 
                {
                    console::Log("[hen::console] Invalid type for " + cvar->Name, console::LOGLEVEL::WARNING);
                    return;
                }

                DisplayValue(cvar);
            }
            return;
        }

        console::Log("[hen::console] Unknown CVar: " + cvarName, console::LOGLEVEL::WARNING);
    }

    static int InputCallback(ImGuiInputTextCallbackData* data)
    {
        if (data->EventChar == '~' || data->EventChar == '`')
        {
            return 1;
        }

        if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
        {
            if (AutocompleteMatches.empty())
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
            else
            {
                if (data->EventKey == ImGuiKey_DownArrow)
                {
                    AutoIndex++;
                    if (AutoIndex >= (int)AutocompleteMatches.size())
                        AutoIndex = 0;
                    return 1;
                }
                else if (data->EventKey == ImGuiKey_UpArrow)
                {
                    AutoIndex--;
                    if (AutoIndex < 0)
                        AutoIndex = (int)AutocompleteMatches.size() - 1;
                    return 1;
                }
            }
        }

        switch (data->EventFlag)
        {
            case ImGuiInputTextFlags_CallbackCompletion:
            {
                if (AutoIndex >= 0 && AutoIndex < (int)AutocompleteMatches.size())
                {
                    const std::string& pick = AutocompleteMatches[AutoIndex];
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, pick.c_str());
                }
                break;
            }
        }

        return 0;
    }

    void Initialise()
    {
        if (!std::filesystem::is_directory("./"))
        {
            bool result = std::filesystem::create_directory("./");
            Initialised = result;
            HEN_ASSERT(result, "Couldn't create log dir");
        }

        LogFile = std::ofstream("./console.txt", std::ios::out); // overwrite any existing file
        HEN_ASSERT(LogFile.is_open(), "Failed to open log file");

        ui::GetIMGUIManager()->RegisterDrawCallback([]() 
        {
            if (!Visible || Locked)
            {
                return;
            }
        
            ImGui::Begin("Console", &Visible, ImGuiWindowFlags_NoDocking);
        
            float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        
            ImGui::BeginChild("Scrolling Region", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
            for(auto &entry : Entries)
            {
                ImVec4 color;
                switch (entry.Level)
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
        
            if (!Locked && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            {
                ImGui::SetScrollHereY(1.0f);
            }
        
            ImGui::EndChild();
            ImGui::PushItemWidth(-1);
        
            if (ImGui::IsWindowAppearing())
            {
                ImGui::SetKeyboardFocusHere();
            }

            ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_CallbackCompletion;
        
            if (ImGui::InputText(" ", InputBuffer, IM_ARRAYSIZE(InputBuffer), flags, InputCallback))
            {
                std::string line(InputBuffer);
                if (!line.empty())
                {
                    Log(std::string("[User] ") + InputBuffer, LOGLEVEL::INFO);
                    
                    if (line.find("log ") != std::string::npos)
                    {
                        //Dont execute because this is a command for letting [User] write something into log                        
                    }
                    else if (line.find("quit") != std::string::npos)
                    {
                        SDL_Event event;
                        event.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&event);
                    }
                    else
                    {
                        if (CommandHistory.empty() || CommandHistory.back() != InputBuffer)
                        {
                            CommandHistory.push_back(InputBuffer);
                        }
                    
                        HistoryIndex = -1;

                        Execute(line);
                    }
                
                    InputBuffer[0] = '\0';
                
                    ImGui::SetKeyboardFocusHere(-1);
                }
            }
        
            ImGui::PopItemWidth();
        
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            std::string currentInput(InputBuffer);
            if (!currentInput.empty())
            {
                cvar::System& system = *cvar::GetSystem();
                std::vector<std::string> allCVarNames = system.GetAllCVarNames();
            
                AutocompleteMatches.clear();
                AutocompleteDisplay.clear();
            
                for (auto& name : allCVarNames)
                {
                    if (name.find(currentInput) == 0 && system.GetCVar(name))
                    {
                        std::string valueStr;
                    
                        std::visit([&](auto&& val)
                        {
                            using T = std::decay_t<decltype(val)>;
                            if constexpr (std::is_same_v<T, bool>)
                            {
                                valueStr = (val ? "true" : "false");
                            }
                            else if constexpr (std::is_same_v<T, std::string>)
                            {
                                valueStr = val;
                            }
                            else
                            {
                                valueStr = std::to_string(val);
                            }
                        }, system.GetCVar(name)->Value);
                    
                        AutocompleteMatches.push_back(name);
                        AutocompleteDisplay.push_back(name + " " + valueStr);
                    }
                }
            
                if (!AutocompleteMatches.empty())
                {
                    float lineHeight = ImGui::GetTextLineHeightWithSpacing();
                    float overlayHeight = AutocompleteDisplay.size() * lineHeight;
                    float maxWidth = 0.0f;
                
                    for (auto& m : AutocompleteDisplay)
                    {
                        float w = ImGui::CalcTextSize(m.c_str()).x;
                        if (w > maxWidth) 
                        {
                            maxWidth = w;
                        }
                    }
                
                    ImVec2 inputMin = ImGui::GetItemRectMin();
                    ImVec2 pos = ImVec2(inputMin.x, inputMin.y - overlayHeight);
                    ImVec2 size = ImVec2(maxWidth + 12.0f, overlayHeight);
                
                    ImU32 bgColour = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
                    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColour, 4.0f);
                
                    for (size_t i = 0; i < AutocompleteDisplay.size(); i++)
                    {
                        ImVec2 itemMin(pos.x, pos.y + i * lineHeight);
                        ImVec2 itemMax(pos.x + size.x, pos.y + (i+1) * lineHeight);
                    
                        if ((int)i == AutoIndex)
                        {
                            drawList->AddRectFilled(itemMin, itemMax, IM_COL32(100,100,120,200));
                        }
                    
                        drawList->AddText(ImVec2(itemMin.x + 4.0f, itemMin.y + 2.0f), IM_COL32(255,255,255,255), AutocompleteDisplay[i].c_str());
                    }
                }
                else
                {
                    AutoIndex = -1;
                }
            }
        
            ImGui::End();
        });

        Initialised = true;
    }

    void Shutdown()
    {
        if (LogFile.is_open())
        {
            LogFile.close();
        }
    }

    void Toggle()
    {
        Visible = !Visible;
    }

    void SetLocked(bool lock)
    {
        Locked = lock;
    }

    void Log(const std::string& message, LOGLEVEL level) 
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

        if (Entries.size() > MAX_DISPLAYED_LOGS)
        {
            Entries.pop_front();
        }

        LogFile << levelText << message << " - " << GetCurrentTime() << "\n";

        #if DEBUG
            std::cout << textColour << levelText << message  << " - " << GetCurrentTime() << "\033[0m\n" <<  std::flush;
        #endif // !DEBUG
    }
}