#include "core/henApplication.h"

#include "core/henVersion.h"
#include "core/henCVar.h"
#include "core/henJobSystem.h"
#include "core/henTimer.h"
#include "core/henArguments.h"
#include "input/henInput.h"
#include "physics/henPhysics.h"
#include "renderer/henRenderer.h"
#include "tools/henConsole.h"
#include "ui/henUI.h"

#include <memory>
#include <fstream>
#include <thread>
#include <algorithm>

#if PLATFORM_WINDOWS
    #define NOMINMAX

    #include <windows.h>
    #include <psapi.h>

    float GetStackUsage() 
    {
        ULONG_PTR low, high;
        GetCurrentThreadStackLimits(&low, &high);

        char current;
        uintptr_t currentPtr = reinterpret_cast<uintptr_t>(&current);

        return (currentPtr - low) / 1024.0f;
    }

    float GetHeapUsage()
    {
        HANDLE hHeap = GetProcessHeap();
        PROCESS_HEAP_ENTRY entry;
        entry.lpData = NULL;
        
        size_t totalAllocated = 0;
        
        while (HeapWalk(hHeap, &entry))
        {
            if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
            {
                totalAllocated += entry.cbData;
            }
        }
    
        return totalAllocated / (1024.0f * 1024.0f);
    }
#else
    #include <malloc.h>

    float GetStackUsage() 
    {
        pthread_attr_t attr;
        pthread_getattr_np(pthread_self(), &attr);

        void* stackBase;
        size_t stackSize;
        pthread_attr_getstack(&attr, &stackBase, &stackSize);

        pthread_attr_destroy(&attr);

        char current;
        uintptr_t currentPtr = reinterpret_cast<uintptr_t>(&current);
        uintptr_t stackTop = static_cast<uintptr_t>(stackBase) + stackSize;

        return (stackTop - currentPtr) / 1024.0f;
    }

    float GetHeapUsage()
    {
        struct mallinfo info = mallinfo();
        return info.uordblks / (1024.0f * 1024.0f);
    }
#endif // !PLATFORM_WINDOWS


namespace hen
{
    static double Accumulator = 0.0;
    static double CurrentTimestep = 0.0;

    static std::string CPUName;
    static SDL_Window* Window;

    static cvar::System CurrentCVarSystem;
    static ui::IMGUIManager CurrentImGuiManager;
    
    cvar::CVar cvar_Fullscreen("a_fullscreen", false, hen::cvar::FLAGS_ARCHIVE, []()
    {
        if (Window)
        {
            SDL_SetWindowFullscreen(Window, cvar_Fullscreen.GetBool());
        }
    });

    cvar::CVar cvar_HZ("a_hz", 60);

    void Application::Initialise(SDL_Window* window)
    {
        Timer timer;   

        console::Initialise(); 

        jobsystem::Initialise();

        HEN_ASSERT(jobsystem::Initialised, "hen::jobsystem not initialised");


        HEN_ASSERT(window != nullptr, "Window is nullptr");

        Window = window;
        SDL_SetWindowFullscreen(Window, cvar_Fullscreen.GetBool());


        renderer::Initialise(window);

        HEN_ASSERT(renderer::Initialised, "hen::renderer not initialised");

        
        physics::Initialise();

        HEN_ASSERT(physics::Initialised, "hen::physics not initialised");
        

        ui::GetIMGUIManager() = &CurrentImGuiManager; 

        CurrentImGuiManager.Initialise(window);

        HEN_ASSERT(CurrentImGuiManager.Initialised, "hen::ui::ImGuiManager not initialised");


        input::Initialise(window);

        HEN_ASSERT(input::Initialised, "hen::input not initialised");


        cvar::GetSystem() = &CurrentCVarSystem; 

        CurrentCVarSystem.Initialise();

        HEN_ASSERT(CurrentCVarSystem.Initialised, "hen::cvar::System not initialised");
        

        Initialised = true;

        std::string infoStr;
        infoStr += "[hen::Application] Initialised with HEN Engine " + version::Version;
            
        #if PLATFORM_WINDOWS
            infoStr += " WINDOWS_";
        #elif PLATFORM_LINUX
            infoStr += " LINUX_";
        #endif // !PLATFORM_WINDOWS

        #if DEBUG
            infoStr += "DEBUG";
        #elif RELEASE
            infoStr += "RELEASE";
        #endif // !DEBUG

        infoStr += " in " + std::to_string(static_cast<int>(std::round(timer.ElapsedMilliseconds()))) + " ms";

        HEN_LOG(infoStr);

        #if PLATFORM_WINDOWS
            char cpuName[256] = {0};
            DWORD size = sizeof(cpuName);

            if (RegGetValueA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString", RRF_RT_REG_SZ, nullptr, cpuName, &size) == ERROR_SUCCESS)
            {
                CPUName = cpuName;
            }
        #endif // !PLATFORM_WINDOWS

        #if PLATFORM_LINUX
            std::ifstream file("/proc/cpuinfo");
            if (file.is_open())
            {
                std::string line;
                while (std::getline(file, line))
                {
                    if (line.rfind("model name", 0) == 0)
                    {
                        auto pos = line.find(':');
                        if (pos != std::string::npos)
                        {
                            CPUName = line.substr(pos + 2);
                            break;
                        }
                    }
                }
            }
        #endif // !PLATFORM_LINUX

        HEN_LOG("[hen::Application] Detected hardware: \n\n------------------------------------\nCPU \n------------------------------------ \n " + CPUName + "\n " + std::to_string(std::thread::hardware_concurrency()) + " Threads \n\n------------------------------------\nGPU \n------------------------------------ \n " + renderer::GetRHC()->GetGPUName() + "\n " + renderer::GetRHC()->GetGPUVendor() + "\n " + renderer::GetRHC()->GetAPIVersion());

        #if DEBUG

            CurrentImGuiManager.RegisterDrawCallback([]() 
            {
                int windowWidth, windowHeight;
                SDL_GetWindowSize(renderer::GetRHC()->GetWindow(), &windowWidth, &windowHeight);
                graphics::Viewport viewport = renderer::GetRHC()->GetViewport();

                ImGui::SetNextWindowPos(ImVec2(viewport.Position.x + 10, (windowHeight - viewport.Position.y - viewport.Size.y) + 10)); 
                ImGui::SetNextWindowBgAlpha(0.0f);

                ImGui::Begin("##infodisplay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);

                ImGui::SetWindowFontScale(1.2f);

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "THIS IS A DEBUG BUILD OF HEN ENGINE %s, PERFORMANCE WILL BE SLOW", version::Version.c_str());
                
                switch (renderer::CurrentBackend)
                {
                    case renderer::BACKENDS::OPENGL:
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "RENDER HARDWARE CONTEXT: OPENGL");
                        break;
                    case renderer::BACKENDS::VULKAN:
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "HAHA MOTHERFUCKERRRR, YOU CAN'T HAVE VULKAN, YOU WILL USE OPENGL");
                        break;
                }

                if (arguments::HasArgument("debugcontext"))
                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "DEBUG RENDER HARDWARE CONTEXT: ENABLED");
                }
                else 
                {
                   ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "DEBUG RENDER HARDWARE CONTEXT: DISABLED");
                }

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "STACK USAGE: %.2f KB", GetStackUsage());
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "HEAP USAGE: %.2f MB", GetHeapUsage());

                ImGui::SetWindowFontScale(1.0f);

                ImGui::End();
            }, true);

        #endif // !DEBUG
    }

    void Application::Shutdown()
    {
        HEN_LOG("[hen::Application] Shutting down all subsystems...");

        ui::GetIMGUIManager()->Shutdown();
        cvar::GetSystem()->Shutdown();

        physics::Shutdown();
        console::Shutdown();
    }

    void Application::Run()
    {
        if (!Initialised)
        {
            return;
        }

        renderer::Run();

        double newTime = static_cast<double>(SDL_GetPerformanceCounter()) / static_cast<double>(SDL_GetPerformanceFrequency());
        double deltaTime = newTime - CurrentTimestep;
        CurrentTimestep = newTime;

        deltaTime = math::Clamp(deltaTime, 0.0f, 0.5f);

        Update(static_cast<float>(deltaTime));

        Accumulator += deltaTime;

        while (Accumulator >= (1.0 / cvar_HZ.GetInt()))
        {
            FixedUpdate();
            Accumulator -= (1.0 / cvar_HZ.GetInt());
        }
    }

    void Application::FixedUpdate()
    {

    }

    void Application::Update(float deltaTime)
    {
        input::Update();
        physics::Update(deltaTime);

        if (input::Press(input::KEYBOARD_BUTTON_TILDE))
        {
            console::Toggle();
        }
    }

    void Application::ProcessEvent(const SDL_Event& event)
    {
        input::ProcessEvent(event);
        CurrentImGuiManager.ProcessEvent(event);
    }
} 