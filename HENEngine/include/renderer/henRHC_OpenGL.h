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
        void ClearSwapChain() override;
        void SwapSwapChain() override;

        SDL_Window* GetWindow() override;
        void ResizeWindow() override;
        
    public:

        bool Initialised =  false;

    private:

        SDL_Window* m_Window;
    };

    
}

#endif // !_HENRHC_OPENGL_H_
