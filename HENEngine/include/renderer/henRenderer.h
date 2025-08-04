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

    inline RHC*& GetRHC()
	{
		static RHC* context = nullptr;
		return context;
	}

    enum class BACKEND
    {
        NONE = 0,
        OPENGL = 1
    };

    extern bool Initialised;
    extern BACKEND CurrentBackend;

}

#endif // !_HENRENDER_H_