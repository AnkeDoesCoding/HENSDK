#ifndef _HENRENDER_H_
#define _HENRENDER_H_

#include "SDL3/SDL.h"

namespace hen::renderer
{
    void Initialise();
    void Run();

    void SetWindow(SDL_Window* window);

    extern SDL_Window* g_Window;
    extern bool Initialised;
}

#endif // !_HENRENDER_H_