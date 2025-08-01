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

        HEN_CONSOLE_ASSERT(window != nullptr, "[hen::Application] Window is nullptr");

        renderer::Initialise(window);

        input::Initialise(renderer::GetRHC()->GetWindow());

        console::Post("[hen::Application] Initialised with HEN Engine " + version::VERSION + " in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");

        input::LockMouse();
        
        Initialised = true;
    }

    void Application::Shutdown()
    {
        console::Post("[hen::Application] Shutting down...");
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

    void Application::Update(float dT)
    {
        renderer::Update(dT);
    }

    void Application::ResizeWindow()
    {
        if(renderer::GetRHC() != nullptr)
        {
            renderer::GetRHC()->ResizeWindow();
        }
    }
} 