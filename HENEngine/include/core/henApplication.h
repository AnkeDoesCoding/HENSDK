#ifndef _HENAPPLICATION_H_
#define _HENAPPLICATION_H_

#include "SDL3/SDL.h"

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
        virtual void Update(double dT);

        void SetWindow(SDL_Window* window);
        SDL_Window* CreateWindow(const char* windowName, int w, int h); // Creates SDL_Window which is configured to use OpenGL

    public:
        
        bool Initialised = false;


    private:
        SDL_Window* m_Window;
    };
}

#endif // !_HENAPPLICATION_H_