#ifndef _HENRENDER_H_
#define _HENRENDER_H_

#include "SDL3/SDL.h"

#include "vendor/glad/include/glad.h"
#include "renderer/henRHC.h"

#include "scene/henScene.h"

namespace hen::renderer
{
    enum class BACKEND
    {
        NONE,
        OPENGL,
        VULKAN
    };

    enum class PRIMITIVES
    {
        CUBE,
        SPHERE
    };

    void Initialise(SDL_Window* window);
    void Run();
    void Update(float deltaTime);
    void ProcessEvent(const SDL_Event& event);

    void RenderPrimitive(PRIMITIVES primitve, glm::vec3 position, glm::vec3 colour);

    inline RHC*& GetRHC()
	{
		static RHC* context = nullptr;
		return context;
	}

    
    extern bool Initialised;
    extern BACKEND CurrentBackend;
    extern scene::actors::Camera Camera;
}

#endif // !_HENRENDER_H_