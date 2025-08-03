#ifndef _HENRENDER_H_
#define _HENRENDER_H_

#include "SDL3/SDL.h"

#include "vendor/glad/include/glad.h"
#include "renderer/henRHC.h"

namespace hen::renderer
{
    void Initialise(SDL_Window* window);
    void Run();
    void Update(float deltaTime);

    extern bool Initialised;
    
    inline RHC*& GetRHC()
	{
		static RHC* context = nullptr;
		return context;
	}

}

#endif // !_HENRENDER_H_