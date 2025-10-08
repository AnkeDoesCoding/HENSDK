#include "core/henApplication.h"

#include "core/henVersion.h"
#include "core/henTimer.h"
#include "core/henCVar.h"
#include "input/henInput.h"
#include "level/henLevel.h"
#include "renderer/henRenderer.h"
#include "tools/henConsole.h"
#include "ui/henUI.h"

#include <memory>

namespace hen
{
    static Uint64 LastTick, CurrentTick = 0;
    static std::unique_ptr<cvar::System> CVarSystem;
    static std::unique_ptr<ui::IMGUIManager> ImGuiManager;

    level::Level test;

    Application::Application()
    {

    }

    Application::~Application()
    {

    }

    void Application::Initialise(SDL_Window* window)
    {
        Timer timer;

        CVarSystem = std::make_unique<cvar::System>(); // this motherfucker is important as fuck
        cvar::GetSystem() = CVarSystem.get();    

        console::Initialise(); 

        HEN_ASSERT(window != nullptr, "[hen::Application] Window is nullptr");

        renderer::Initialise(window);

        ImGuiManager = std::make_unique<ui::IMGUIManager>();
        ui::GetIMGUIManager() = ImGuiManager.get(); 

        ImGuiManager->Initialise(window);

        input::Initialise(renderer::GetRHC()->GetWindow());

        if(input::Initialised && renderer::Initialised && console::Initialised)
        {
            Initialised = true;

            std::string infoStr;
            infoStr += "[hen::Application] Initialised with HEN Engine " + version::VERSION;
            
            #if PLATFORM_WINDOWS
                infoStr += " WINDOWS ";
            #elif PLATFORM_LINUX
                infoStr += " LINUX ";
            #endif // !PLATFORM_WINDOWS

            #if DEBUG
                infoStr += "(DEBUG)";
            #elif RELEASE
                infoStr += "(RELEASE)";
            #endif // !DEBUG

            infoStr += " in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms";

            console::Log(infoStr, console::LOGLEVEL::INFO);
        }
    }

    void Application::Shutdown()
    {
        console::Log("[hen::Application] Shutting down...");
        console::Shutdown();

        ui::GetIMGUIManager()->Shutdown();
    }

    void Application::Run()
    {
        if(Initialised)
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
        if(input::Press(input::KEYBOARD_BUTTON_TILDE))
        {
            console::Toggle();
        }
    }

    void Application::Update(float deltaTime)
    {
        renderer::Update(deltaTime);
    }

    void Application::ProcessEvent(const SDL_Event& event)
    {
        input::ProcessEvent(event);
        ImGuiManager->ProcessEvent(event);
    }

    void Application::ResizeWindow()
    {
        if(renderer::GetRHC() != nullptr)
        {
            renderer::GetRHC()->ResizeWindow();
        }
    }
} 