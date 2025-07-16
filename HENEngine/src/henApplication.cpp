#include "core/henApplication.h"

#include "henCommonInclude.h"

#include "tools/henConsole.h"
#include "core/henVersion.h"
#include "renderer/henRenderer.h"

#include "henRHC_OpenGL.h"

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

            float deltaTime = float(Timer.RecordElapsedSeconds());
            const float targetDT = 1.0f / 60; // 60 hz
            deltaTime = Clamp(deltaTime, 0.0f, 0.5f); // clamp to prevent instability

            Update(deltaTime);
            FixedUpdate();
        }
    }

    void Application::FixedUpdate()
    {
        
    }

    void Application::Update(float dT)
    {
        
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