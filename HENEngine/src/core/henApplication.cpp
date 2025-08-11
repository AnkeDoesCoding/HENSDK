#include "core/henApplication.h"

#include "core/henVersion.h"
#include "core/henTimer.h"
#include "input/henInput.h"
#include "renderer/henRenderer.h"
#include "tools/henConsole.h"

Uint64 LastTick, CurrentTick = 0;

namespace hen
{
    Application::Application()
    {

    }

    Application::~Application()
    {

    }

    void Application::Initialise(SDL_Window* window)
    {
        Timer timer;

        console::Initialise(); // this motherfucker is important as fuck

        HEN_ASSERT(window != nullptr, "[hen::Application] Window is nullptr");

        renderer::Initialise(window);

        input::Initialise(renderer::GetRHC()->GetWindow());

        if(input::Initialised && renderer::Initialised && console::Initialised)
        {
            Initialised = true;
            #if DEBUG
                console::Log("[hen::Application] Initialised with HEN Engine " + version::VERSION + " (DEBUG)" + " in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
            #elif RELEASE
                console::Log("[hen::Application] Initialised with HEN Engine " + version::VERSION + " (RELEASE)" + " in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
            #endif // !DEBUG
        }
    }

    void Application::Shutdown()
    {
        console::Log("[hen::Application] Shutting down...");
        console::Shutdown();
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
        
    }

    void Application::Update(float deltaTime)
    {
        renderer::Update(deltaTime);
    }

    void Application::ProcessEvent(const SDL_Event& event)
    {
        input::ProcessEvent(event);
        renderer::ProcessEvent(event);
    }

    void Application::ResizeWindow()
    {
        if(renderer::GetRHC() != nullptr)
        {
            renderer::GetRHC()->ResizeWindow();
        }
    }
} 