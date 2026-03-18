#ifndef _HENRHC_OPENGL_H_
#define _HENRHC_OPENGL_H_

#include "renderer/henRHC.h"

namespace hen
{
    class RHC_OpenGL : public RHC
    {
    public:
        RHC_OpenGL(SDL_Window* window);

        std::string GetGPUName() override;
        std::string GetGPUVendor() override;
        std::string GetAPIVersion() override;

        void Initialise() override;
        void Clear() override;
        void Present() override;
        
        void DrawElements(uint32_t count, uint32_t offset) override;
        void DrawArrays(uint32_t count, uint32_t offset) override;

        void EnableDepth() override;
        void DisableDepth() override;
        void ClearDepth() override;
        void EnableDepthMask() override;
        void DisableDepthMask() override;
        void SetDepthMask(graphics::DEPTH_FUNCTIONS function) override;

        void EnableVSync() override;
        void DisableVSync() override;

        void EnableStencil() override;
        void DisableStencil() override;
        void EnableStencilMask() override;
        void DisableStencilMask() override;
        void SetStencilMask(graphics::DEPTH_FUNCTIONS function, int reference, uint32_t mask) override;
        void SetStencilOperation(graphics::STENCIL_FUNCTIONS stencilFail, graphics::STENCIL_FUNCTIONS depthFail, graphics::STENCIL_FUNCTIONS pass) override;

        void EnableFaceCulling() override;
        void DisableFaceCulling() override;
        void SetCulledFace(graphics::CULL_MODES face) override;

        SDL_Window* GetWindow() const override;
        void ResizeWindow() override;
        
    public:
        bool Initialised =  false;

    private:
        SDL_Window* m_Window;
    };

    
}

#endif // !_HENRHC_OPENGL_H_
