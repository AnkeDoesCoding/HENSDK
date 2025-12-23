#include "core/henApplication.h"

#include "core/henVersion.h"
#include "core/henCVar.h"
#include "input/henInput.h"
#include "level/henLevel.h"
#include "renderer/henRenderer.h"
#include "tools/henConsole.h"
#include "ui/henUI.h"

#include <memory>

namespace hen
{
    static uint64_t LastTick, CurrentTick = 0;
    static SDL_Window* Window;
    static std::unique_ptr<cvar::System> CurrentCVarSystem;
    static std::unique_ptr<ui::IMGUIManager> CurrentImGuiManager;
    
    cvar::CVar cvar_Fullscreen("a_fullscreen", false, hen::cvar::FLAGS_ARCHIVE, []()
    {
        if (Window)
        {
            SDL_SetWindowFullscreen(Window, cvar_Fullscreen.GetBool());
        }
    });

    Application::Application()
    {

    }

    Application::~Application()
    {

    }

    void Application::Initialise(SDL_Window* window)
    {
        Timer timer;   

        console::Initialise(); 

        HEN_ASSERT(window != nullptr, "Window is nullptr");

        Window = window;


        renderer::Initialise(window);

        HEN_ASSERT(renderer::Initialised, "hen::renderer not initialised");


        CurrentImGuiManager = std::make_unique<ui::IMGUIManager>();
        ui::GetIMGUIManager() = CurrentImGuiManager.get(); 

        CurrentImGuiManager->Initialise(window);

        HEN_ASSERT(CurrentImGuiManager->Initialised, "hen::ui::ImGuiManager not initialised");


        input::Initialise(renderer::GetRHC()->GetWindow());

        HEN_ASSERT(input::Initialised, "hen::input not initialised");


        CurrentCVarSystem = std::make_unique<cvar::System>(); // this motherfucker is important as fuck
        cvar::GetSystem() = CurrentCVarSystem.get(); 

        CurrentCVarSystem->Initialise();

        HEN_ASSERT(CurrentCVarSystem->Initialised, "hen::cvar::System not initialised");
        

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

        infoStr += " in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms";

        SDL_SetWindowFullscreen(Window, cvar_Fullscreen.GetBool());

        HEN_LOG(infoStr);
    }

    void Application::Shutdown()
    {
        HEN_LOG("[hen::Application] Shutting down...");

        ui::GetIMGUIManager()->Shutdown();
        cvar::GetSystem()->Shutdown();

        console::Shutdown();
    }

    void Application::Run()
    {
        if (Initialised)
        {
            renderer::Run();

            LastTick = CurrentTick;
            CurrentTick = SDL_GetTicks();

            float deltaTime = (CurrentTick - LastTick) / 1000.0f;

            Update(deltaTime);
            FixedUpdate();

            input::Update();
        }
    }

    void Application::FixedUpdate()
    {
        if (input::Press(input::KEYBOARD_BUTTON_TILDE))
        {
            console::Toggle();
        }
    }

    void Application::Update(float deltaTime)
    {

    }

    void Application::ProcessEvent(const SDL_Event& event)
    {
        input::ProcessEvent(event);
        CurrentImGuiManager->ProcessEvent(event);
    }

    void Application::ResizeWindow()
    {
        if (renderer::GetRHC() != nullptr)
        {
            renderer::GetRHC()->ResizeWindow();
        }
    }
} 