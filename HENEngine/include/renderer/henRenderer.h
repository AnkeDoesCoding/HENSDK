#ifndef _HENRENDER_H_
#define _HENRENDER_H_

#include "SDL3/SDL.h"

#include "vendor/glad/include/glad.h"
#include "renderer/henRenderHardwareContext.h"

namespace hen::renderer
{
    void Initialise();
    void Run();

    extern bool Initialised;
    
    inline RenderHardwareContext*& GetRHC()
	{
		static RenderHardwareContext* context = nullptr;
		return context;
	}
}

#endif // !_HENRENDER_H_