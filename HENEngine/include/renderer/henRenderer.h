#ifndef _HENRENDER_H_
#define _HENRENDER_H_

#include "SDL3/SDL.h"

#include "vendor/glad/include/glad.h"
#include "renderer/henRHC.h"

#include "scene/henScene.h"

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
        NONE,
        OPENGL
    };

    extern bool Initialised;
    extern BACKEND CurrentBackend;
    extern scene::actors::Camera Camera;
}

#endif // !_HENRENDER_H_