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
        virtual void Clear() = 0;
        virtual void Present() = 0;

        virtual void EnableDepth() = 0;
        virtual void DisableDepth() = 0;

        virtual void EnableVSync() = 0;
        virtual void DisableVSync() = 0;

        virtual void EnableBackFaceCulling() = 0;
        virtual void DisableBackFaceCulling() = 0;

        virtual SDL_Window* GetWindow() const = 0;
        virtual void ResizeWindow() = 0;
    };

   
}

#endif // !_HENRHC_H_