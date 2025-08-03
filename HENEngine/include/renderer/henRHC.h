#ifndef _HENRHC_H_
#define _HENRHC_H_

#include "SDL3/SDL.h"

namespace hen
{
    class RHC
    {
    public:

        virtual ~RHC() = default;

        virtual void Initialise() = 0;
        virtual void ClearSwapChain() = 0;
        virtual void SwapSwapChain() = 0;

        virtual SDL_Window* GetWindow() = 0;
        virtual void ResizeWindow() = 0;
    };

   
}

#endif // !_HENRHC_H_