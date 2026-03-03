#include "tools/henConsole.h"

#include "core/henCVar.h"
#include "ui/henUI.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <deque>
#include <vector>
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
    static std::deque<LogEntry> PendingEntries;
    static std::deque<LogEntry> Entries;

    static std::mutex Mutex;

    static char InputBuffer[1024] = "";

    static std::vector<std::string> AutocompleteMatches;
    static std::vector<std::string> AutocompleteDisplay;
    static int AutoCompleteIndex = -1;

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

        HEN_WARN("[hen::console] Invalid bool string: " + inputString);
        return false;
    }

    static void DisplayValue(cvar::CVar* cvar)
    {
        std::visit([&](auto&& val)
        {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>)
            {
                HEN_LOG("[hen::console] " + cvar->Name + " = " + val);
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                HEN_LOG("[hen::console] " + cvar->Name + " = " + std::string(val ? "true" : "false"));
            }
            else
            {
                HEN_LOG("[hen::console] " + cvar->Name + " = " + std::to_string(val));
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
                    HEN_WARN("[hen::console] Protection is enabled, type protection_enabled 0 to disable it");
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
                    HEN_ERROR("[hen::console] Invalid type for " + cvar->Name);
                    return;
                }

                DisplayValue(cvar);
            }
            return;
        }

        HEN_WARN("[hen::console] Unknown CVar: " + cvarName);
    }

    static int InputCallback(ImGuiInputTextCallbackData* data)
    {
        if(data->EventChar == '~' || data->EventChar == '`')
        {
            return 1;
        } 
         
        switch (data->EventFlag)
        {
            case ImGuiInputTextFlags_CallbackCompletion:
                if (AutoCompleteIndex >= 0 && AutoCompleteIndex < static_cast<int>(AutocompleteMatches.size()))
                {
                    const std::string& pick = AutocompleteMatches[AutoCompleteIndex];
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, pick.c_str());
                }
                break;
            case ImGuiInputTextFlags_CallbackHistory:
                if (data->EventKey == ImGuiKey_DownArrow)
                {
                    AutoCompleteIndex++;
                    if (AutoCompleteIndex >= static_cast<int>(AutocompleteMatches.size()))
                    {
                        AutoCompleteIndex = 0;
                    }
                    return 1;
                }
                else if (data->EventKey == ImGuiKey_UpArrow)
                {
                    AutoCompleteIndex--;
                    if (AutoCompleteIndex < 0)
                    {
                        AutoCompleteIndex = static_cast<int>(AutocompleteMatches.size() - 1);
                    }
                    return 1;
                }
                break;
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
        
            {
                std::scoped_lock lock(Mutex);

                while(!PendingEntries.empty())
                {
                    Entries.push_back(PendingEntries.front());
                    PendingEntries.pop_front();
                
                    if(Entries.size() > MAX_DISPLAYED_LOGS)
                    {
                        Entries.pop_front();
                    }
                }
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
                case LOGLEVEL::ASSERT:
                    color = ImVec4(1,0,0,1);                    
                    break;
                default:
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

            ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackCharFilter;
        
            if (ImGui::InputText(" ", InputBuffer, IM_ARRAYSIZE(InputBuffer), flags, InputCallback))
            {
                std::string line(InputBuffer);
                if (!line.empty())
                {
                    if (line.find("log") != std::string::npos)
                    {
                        //Dont execute because this is a command for letting [User] write something into log 

                        const std::string toRemove = "log ";
                        size_t pos = line.find(toRemove);
                        if (pos != std::string::npos)
                        {
                            line.erase(pos, toRemove.length());
                        }

                        Log("[User] " + line, LOGLEVEL::INFO);

                    }
                    else if (line.find("list") != std::string::npos)
                    {
                        Log("[User] " + line, LOGLEVEL::INFO);
                        Log(cvar::GetSystem()->ListCVars());
                    }
                    else if (line.find("quit") != std::string::npos)
                    {
                        Log("[User] " + line, LOGLEVEL::INFO);

                        SDL_Event event;
                        event.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&event);
                    }
                    else
                    {
                        Log("[User] " + line, LOGLEVEL::INFO);

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
                    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColour, 0.0f);
                
                    for (size_t i = 0; i < AutocompleteDisplay.size(); i++)
                    {
                        ImVec2 itemMin(pos.x, pos.y + i * lineHeight);
                        ImVec2 itemMax(pos.x + size.x, pos.y + (i+1) * lineHeight);
                    
                        if (static_cast<int>(i) == AutoCompleteIndex)
                        {
                            drawList->AddRectFilled(itemMin, itemMax, IM_COL32(100,100,120,200));
                        }
                    
                        drawList->AddText(ImVec2(itemMin.x + 4.0f, itemMin.y + 2.0f), IM_COL32(255,255,255,255), AutocompleteDisplay[i].c_str());
                    }
                }
                else
                {
                    AutoCompleteIndex = -1;
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
        case LOGLEVEL::ASSERT:
            levelText = "ASSERTION FAILED: ";
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

        {
            std::scoped_lock lock(Mutex);
            PendingEntries.push_back(entry);
        }

        LogFile << levelText << message << " - " << GetCurrentTime() << "\n";

        #if DEBUG
            std::cout << textColour << levelText << message  << " - " << GetCurrentTime() << "\033[0m\n" <<  std::flush;
        #endif // !DEBUG
    }
}