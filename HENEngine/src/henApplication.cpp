#include "core/henApplication.h"

#include "tools/henConsole.h"
#include "core/henVersion.h"

#include "renderer/henRenderer.h"

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
        }
    }

    void Application::FixedUpdate()
    {
        
    }

    void Application::Update(double dT)
    {
        
    }

    void Application::SetWindow(SDL_Window *window)
    {
        m_Window = window;

        renderer::SetWindow(window);
    }

    SDL_Window* Application::CreateWindow(const char* windowName, int w, int h)
    {
        SDL_Window* newWindow;
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

        newWindow = SDL_CreateWindow(windowName, w, h, SDL_WINDOW_OPENGL);

        SetWindow(newWindow);

        return newWindow;
    }
}