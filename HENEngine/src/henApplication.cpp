#include "core/henApplication.h"

#include "henCommonInclude.h"

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

        renderer::SetWindow(window);

        if (renderer::Initialised)
        {
            renderer::ResizeWindow();
        }
    }

    SDL_Window* Application::CreateWindow(const char* windowName, int w, int h)
    {
        SDL_Window* newWindow;
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

        newWindow = SDL_CreateWindow(windowName, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        SetWindow(newWindow);

        return newWindow;
    }
}