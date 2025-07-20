#include "core/henApplication.h"

#include "henCommonInclude.h"
#include "henRHC_OpenGL.h"

#include "core/henVersion.h"
#include "helpers/henTimer.h"
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

    void Application::Initialise()
    {
        helper::Timer timer;

        RHC = std::make_unique<RHC_OpenGL>(Window);
        renderer::GetRHC() = RHC.get();

        renderer::Initialise();

        input::Initialise(RHC->GetWindow());

        console::Post("[hen::Application] Initialised with HEN Engine " + version::VERSION + " in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");

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

    void Application::SetWindow(SDL_Window *window)
    {
        Window = window;

        if(RHC != nullptr)
        {
            RHC->ResizeWindow();
        }
    }
} 