#ifndef _HENRENDER_H_
#define _HENRENDER_H_

#include "SDL3/SDL.h"

#include "renderer/henRenderHardwareContext.h"

namespace hen::renderer
{
    void Initialise();
    void Run();

    extern bool Initialised;
    
}

#endif // !_HENRENDER_H_