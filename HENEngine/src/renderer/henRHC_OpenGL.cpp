#include "henRHC_OpenGL.h"

#include "vendor/glad/include/glad.h"

#include "core/henArguments.h"
#include "core/henTimer.h"
#include "tools/henConsole.h"

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

    std::string RHC_OpenGL::GetGPUName()
    {
        const GLubyte* renderer = glGetString(GL_RENDERER);
        return renderer ? reinterpret_cast<const char*>(renderer) : "UNKNOWN";
    }

    std::string RHC_OpenGL::GetGPUVendor()
    {
        const GLubyte* vendor = glGetString(GL_VENDOR);
        return vendor ? reinterpret_cast<const char*>(vendor) : "UNKNOWN";
    }

    std::string RHC_OpenGL::GetAPIVersion()
    {
        const GLubyte* version = glGetString(GL_VERSION);
        return version ? reinterpret_cast<const char*>(version) : "UNKNOWN";
    }

    void RHC_OpenGL::Initialise() 
    {
        Timer timer;

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
        if (arguments::HasArgument("debugcontext"))
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
            HEN_LOG("[hen::RHC_OpenGL] OpenGL context will be created in debug mode");
        }

        SDL_GLContext context = SDL_GL_CreateContext(m_Window);
        SDL_GL_MakeCurrent(m_Window, context);

        int loadGL = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

        HEN_ASSERT(loadGL, "Failed to load OpenGL context");

        SDL_GL_SetSwapInterval(0); // create with vysnc off

        Initialised = true;

        HEN_LOG("[hen::RHC_OpenGL] Initialised in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void RHC_OpenGL::Clear()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // just clear everything even if it aint enabled
    }

    void RHC_OpenGL::Present()
    {
        SDL_GL_SwapWindow(m_Window);
    }

    void RHC_OpenGL::DrawElements(uint32_t count, uint32_t offset)
    {
        glDrawElements(GL_TRIANGLES, (GLsizei)count, GL_UNSIGNED_INT, reinterpret_cast<void*>(offset * sizeof(uint32_t)));
    }

    void RHC_OpenGL::DrawArrays(uint32_t count, uint32_t offset)
    {
        glDrawArrays(GL_TRIANGLES, offset, count);
        
    }

    void RHC_OpenGL::EnableDepth()
    {
        glEnable(GL_DEPTH_TEST);  
    }

    void RHC_OpenGL::DisableDepth()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void RHC_OpenGL::EnableVSync()
    {
        SDL_GL_SetSwapInterval(1);
    }

    void RHC_OpenGL::DisableVSync()
    {
        SDL_GL_SetSwapInterval(0);
    }

    void RHC_OpenGL::EnableStencil()
    {
        glEnable(GL_STENCIL_TEST);
    }

    void RHC_OpenGL::DisableStencil()
    {
        glDisable(GL_STENCIL_TEST);
    }

    void RHC_OpenGL::EnableBackFaceCulling()
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void RHC_OpenGL::DisableBackFaceCulling()
    {
        glDisable(GL_CULL_FACE);
    }

    SDL_Window* RHC_OpenGL::GetWindow() const
    {
        return m_Window;
    }

    void RHC_OpenGL::ResizeWindow()
    {
        int newWidth, newHeight;
        SDL_GetWindowSize(m_Window, &newWidth, &newHeight);

        glViewport(0, 0, newWidth, newHeight);
    }

}