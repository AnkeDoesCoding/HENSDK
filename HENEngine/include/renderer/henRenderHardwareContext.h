#ifndef _HENRENDERHARDWARECONTEXT_H_
#define _HENRENDERHARDWARECONTEXT_H_

#include "SDL3/SDL.h"

namespace hen
{
    class RenderHardwareContext
    {
    public:

        virtual ~RenderHardwareContext() = default;

        virtual void Initialise() = 0;
        virtual void ClearSwapChain() = 0;
        virtual void SwapSwapChain() = 0;

        virtual void ResizeWindow() = 0;
    };

   
}

#endif // !_HENRENDERHARDWARECONTEXT_H_