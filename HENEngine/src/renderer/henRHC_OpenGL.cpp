#include "renderer/henRHC_OpenGL.h"

#include "vendor/glad/include/glad.h"

#include "core/henTimer.h"
#include "tools/henConsole.h"

#include <cassert>
#include <cmath>

namespace hen
{
    RHC_OpenGL::RHC_OpenGL(SDL_Window* window)
    {
        m_Window = window;
    }

    RHC_OpenGL::~RHC_OpenGL()
    {
    }

    void RHC_OpenGL::Initialise() 
    {
        Timer timer;

        SDL_GLContext context = SDL_GL_CreateContext(m_Window);
        SDL_GL_MakeCurrent(m_Window, context);

        int loadGL = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

        HEN_ASSERT(loadGL, "[hen::RHC_OpenGL] Failed to load OpenGL context");

        glEnable(GL_DEPTH_TEST);  

        Initialised = true;

        console::Post("[hen::RHC_OpenGL] Initialised in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void RHC_OpenGL::ClearSwapChain()
    {
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RHC_OpenGL::SwapSwapChain()
    {
        SDL_GL_SwapWindow(m_Window);
    }

    SDL_Window* RHC_OpenGL::GetWindow()
    {
        return m_Window;
    }

    void RHC_OpenGL::ResizeWindow()
    {
        int newWidth, newHeight;
        SDL_GetWindowSize(m_Window, &newWidth, &newHeight);

        console::Post(std::to_string(newWidth) + " : " + std::to_string(newHeight));

        glViewport(0, 0, newWidth, newHeight);
    }

}