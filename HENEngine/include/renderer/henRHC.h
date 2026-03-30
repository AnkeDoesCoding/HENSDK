#ifndef _HENRHC_H_
#define _HENRHC_H_

#include "graphics/henGraphics.h"

#include <SDL3/SDL.h>

#include <string>

namespace hen
{
    class RHC
    {
    public:
        virtual std::string GetGPUName() = 0;
        virtual std::string GetGPUVendor() = 0;
        virtual std::string GetAPIVersion() = 0;

        virtual void Initialise() = 0;
        virtual void Clear() = 0;
        virtual void Present() = 0;

        virtual void DrawElements(uint32_t count, uint32_t offset) = 0;
        virtual void DrawArrays(uint32_t count, uint32_t offset) = 0;

        virtual void EnableVSync() = 0;
        virtual void DisableVSync() = 0;

        virtual void EnableFaceCulling() = 0;
        virtual void DisableFaceCulling() = 0;
        virtual void SetCulledFace(graphics::CULL_MODES face) = 0;

        virtual void EnableDepth() = 0;
        virtual void DisableDepth() = 0;
        virtual void ClearDepth() = 0;
        virtual void EnableDepthMask() = 0;
        virtual void DisableDepthMask() = 0;
        virtual void SetDepthMask(graphics::DEPTH_FUNCTIONS function) = 0;

        virtual void EnableStencil() = 0;
        virtual void DisableStencil() = 0;
        virtual void EnableStencilMask() = 0;
        virtual void DisableStencilMask() = 0;
        virtual void SetStencilMask(graphics::DEPTH_FUNCTIONS function, int reference, uint32_t mask) = 0;
        virtual void SetStencilOperation(graphics::STENCIL_FUNCTIONS stencilFail, graphics::STENCIL_FUNCTIONS depthFail, graphics::STENCIL_FUNCTIONS pass) = 0;

        virtual void EnableBlending() = 0;
        virtual void DisableBlending() = 0;
        virtual void SetBlendOperation(graphics::BLEND_FUNCTIONS source, graphics::BLEND_FUNCTIONS destination) = 0;

        virtual SDL_Window* GetWindow() const = 0;
        virtual void ResizeViewport(int posX, int posY, int width, int height) = 0;
        virtual graphics::Viewport GetViewport() const = 0;
    };
   
}

#endif // !_HENRHC_H_