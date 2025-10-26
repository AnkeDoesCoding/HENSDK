#ifndef _HENRENDER_H_
#define _HENRENDER_H_

#include "SDL3/SDL.h"

#include "vendor/glad/include/glad.h"
#include "renderer/henRHC.h"
#include "renderer/henRenderer_ResourceManagers.h"
#include "level/henLevel.h"

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

    void RenderPrimitive(PRIMITIVES primitve, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 colour);

    void RenderLevel();

    inline RHC*& GetRHC()
	{
		static RHC* context = nullptr;
		return context;
	}
    
    extern bool Initialised;
    extern BACKEND CurrentBackend;
    extern level::CameraComponent Camera;
    
}

#endif // !_HENRENDER_H_