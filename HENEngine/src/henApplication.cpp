#include "core/henApplication.h"

#include "tools/henConsole.h"
#include "core/henVersion.h"

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
        if (Initialised)
        {
            console::Post("[hen::Application] Already initialised", console::Level::Warning);

            return;
        }

        console::Post("[hen::Application] Successfully initialised with HEN Engine version : " + version::VERSION);

        Initialised = true;
    }

    void Application::Shutdown()
    {
        console::Post("[hen::Application] Shutting down...");

    }

    void Application::Run()
    {
        
    }

    void Application::FixedUpdate()
    {
        
    }

    void Application::Update(double dT)
    {
        
    }
}