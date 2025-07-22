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

        virtual void Initialise(SDL_Window* window);
        virtual void Shutdown();
        
        void Run();

        virtual void FixedUpdate();
        virtual void Update(float dT);

        void ResizeWindow();

    public:
        
        bool Initialised = false;

        helper::Timer Timer;
    };
}

#endif // !_HENAPPLICATION_H_