#ifndef _HENRENDER_H_
#define _HENRENDER_H_

#include "SDL3/SDL.h"

#include "renderer/henRHC.h"
#include "renderer/henRenderer_ResourceManagers.h"
#include "level/henLevel.h"

namespace hen::renderer
{
    enum class BACKENDS
    {
        OPENGL,
        VULKAN
    };

    void Initialise(SDL_Window* window);
    void Run();

    void PrepareResources();
    void Render();
    void RenderPrimitive(level::PRIMITIVE_TYPES primitve, math::Vec3 position, math::Vec3 rotation, math::Vec3 scale, math::Vec3 colour);

    inline RHC*& GetRHC()
	{
		static RHC* context = nullptr;
		return context;
	}
    
    extern bool Initialised;
    extern BACKENDS CurrentBackend;
    extern level::CameraComponent Camera;
    
}

#endif // !_HENRENDER_H_