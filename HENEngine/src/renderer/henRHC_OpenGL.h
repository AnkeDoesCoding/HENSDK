#ifndef _HENRHC_OPENGL_H_
#define _HENRHC_OPENGL_H_

#include "renderer/henRHC.h"

namespace hen
{
    class RHC_OpenGL : public RHC
    {
    public:

        RHC_OpenGL(SDL_Window* window);
        ~RHC_OpenGL() override;

        void Initialise() override;
        void Clear() override;
        void Present() override;

        void DrawElements(uint32_t count, uint32_t offset) override;
        void DrawArrays(uint32_t count, uint32_t offset) override;

        void EnableDepth() override;
        void DisableDepth() override;

        void EnableVSync() override;
        void DisableVSync() override;

        void EnableStencil() override;
        void DisableStencil() override;

        void EnableBackFaceCulling() override;
        void DisableBackFaceCulling() override;

        SDL_Window* GetWindow() const override;
        void ResizeWindow() override;
        
    public:

        bool Initialised =  false;

    private:

        SDL_Window* m_Window;
    };

    
}

#endif // !_HENRHC_OPENGL_H_
