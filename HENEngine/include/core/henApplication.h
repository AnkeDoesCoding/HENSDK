#ifndef _HENAPPLICATION_H_
#define _HENAPPLICATION_H_

#include "SDL3/SDL.h"

#include "core/henTimer.h"

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
        virtual void Update(float deltaTime);

        void ProcessEvent(const SDL_Event& event);
        
        void ResizeWindow();

    public:
        
        bool Initialised = false;
    };
}

#endif // !_HENAPPLICATION_H_