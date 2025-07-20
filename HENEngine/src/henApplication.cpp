#include "core/henApplication.h"

#include "henCommonInclude.h"

#include "tools/henConsole.h"
#include "core/henVersion.h"
#include "renderer/henRenderer.h"
#include "input/henInput.h"

#include "henRHC_OpenGL.h"

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
        console::Post("[hen::Application] Initialised with HEN Engine " + version::VERSION);

        RHC = std::make_unique<RHC_OpenGL>(Window);
        renderer::GetRHC() = RHC.get();

        renderer::Initialise();

        input::Initialise(RHC->GetWindow());

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