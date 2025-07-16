#ifndef _HENAPPLICATION_H_
#define _HENAPPLICATION_H_

#include "SDL3/SDL.h"

#include "helpers/henTimer.h"
#include "renderer/henRenderHardwareContext.h"

namespace hen
{
    class Application
    {
    public:
        Application();
        ~Application();

        virtual void Initialise();
        virtual void Shutdown();
        
        void Run();

        virtual void FixedUpdate();
        virtual void Update(float dT);

        void SetWindow(SDL_Window* window);

    public:
        
        bool Initialised = false;

        helper::Timer Timer;

        SDL_Window* Window;

        std::unique_ptr<hen::RenderHardwareContext> RHC;
    };
}

#endif // !_HENAPPLICATION_H_