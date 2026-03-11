#include "henRHC_OpenGL.h"

#include "core/henArguments.h"
#include "core/henTimer.h"
#include "tools/henConsole.h"

#include <glad/include/glad.h>

#include <cmath>

namespace hen
{
    RHC_OpenGL::RHC_OpenGL(SDL_Window* window)
        : m_Window(window)
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

        int loadGL = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress));

        HEN_ASSERT(loadGL, "Failed to load OpenGL context");

        SDL_GL_SetSwapInterval(0); // create with vysnc off

        Initialised = true;

        HEN_LOG("[hen::RHC_OpenGL] Initialised in " + std::to_string(static_cast<int>(std::round(timer.ElapsedMilliseconds()))) + " ms");
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
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, reinterpret_cast<void*>(offset * sizeof(uint32_t)));
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

    void RHC_OpenGL::EnableDepthMask()
    {
        glDepthMask(GL_TRUE);
    }

    void RHC_OpenGL::DisableDepthMask()
    {
        glDepthMask(GL_FALSE);
    }

    void RHC_OpenGL::SetDepthMask(graphics::DEPTH_FUNCTIONS function)
    {
        switch (function)
        {
            case graphics::DEPTH_FUNCTIONS::ALWAYS:
                glDepthFunc(GL_ALWAYS);
                break;
            case graphics::DEPTH_FUNCTIONS::NEVER:
                glDepthFunc(GL_NEVER);
                break;
            case graphics::DEPTH_FUNCTIONS::EQUAL:
                glDepthFunc(GL_EQUAL);
                break;
            case graphics::DEPTH_FUNCTIONS::NOT_EQUAL:
                glDepthFunc(GL_NOTEQUAL);
                break;
            case graphics::DEPTH_FUNCTIONS::LESS:
                glDepthFunc(GL_LESS);
                break;
            case graphics::DEPTH_FUNCTIONS::LESS_EQUAL:
                glDepthFunc(GL_LEQUAL);
                break;
            case graphics::DEPTH_FUNCTIONS::GREATER:
                glDepthFunc(GL_GREATER);
                break;
            case graphics::DEPTH_FUNCTIONS::GREATER_EQUAL:
                glDepthFunc(GL_GEQUAL);
                break;
            default:
                glDepthFunc(GL_ALWAYS);
                break;
        }
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

    void RHC_OpenGL::EnableStencilMask()
    {
        glStencilMask(0xFF);
    }

    void RHC_OpenGL::DisableStencilMask()
    {
        glStencilMask(0x00);
    }

    void RHC_OpenGL::SetStencilMask(graphics::DEPTH_FUNCTIONS function, int reference, uint32_t mask)
    {
        GLuint glFunc;

        switch (function)
        {
            case graphics::DEPTH_FUNCTIONS::ALWAYS:
                glFunc = GL_ALWAYS;
                break;
            case graphics::DEPTH_FUNCTIONS::NEVER:
                glFunc = GL_NEVER;
                break;
            case graphics::DEPTH_FUNCTIONS::EQUAL:
                glFunc = GL_EQUAL;
                break;
            case graphics::DEPTH_FUNCTIONS::NOT_EQUAL:
                glFunc = GL_NOTEQUAL;
                break;
            case graphics::DEPTH_FUNCTIONS::LESS:
                glFunc = GL_LESS;
                break;
            case graphics::DEPTH_FUNCTIONS::LESS_EQUAL:
                glFunc = GL_LEQUAL;
                break;
            case graphics::DEPTH_FUNCTIONS::GREATER:
                glFunc = GL_GREATER;
                break;
            case graphics::DEPTH_FUNCTIONS::GREATER_EQUAL:
                glFunc = GL_GEQUAL;
                break;
            default:
                glFunc = GL_ALWAYS;
                break;
        }

        glStencilFunc(glFunc, reference, mask);
    }

    void RHC_OpenGL::SetStencilOperation(graphics::STENCIL_FUNCTIONS stencilFail, graphics::STENCIL_FUNCTIONS depthFail, graphics::STENCIL_FUNCTIONS pass)
    {
        GLuint glStencilFail;
        GLuint glDepthFail;
        GLuint glPass;

        switch (stencilFail)
        {
            case graphics::STENCIL_FUNCTIONS::KEEP:
                glStencilFail = GL_KEEP;
                break;
            case graphics::STENCIL_FUNCTIONS::ZERO:
                glStencilFail = GL_ZERO;                        
                break;
            case graphics::STENCIL_FUNCTIONS::REPLACE:
                glStencilFail = GL_REPLACE;                
                break;
            case graphics::STENCIL_FUNCTIONS::INCREASE:
                glStencilFail = GL_INCR;                
                break;
            case graphics::STENCIL_FUNCTIONS::INCREASE_WRAP:
                glStencilFail = GL_INCR_WRAP;                
                break;
            case graphics::STENCIL_FUNCTIONS::DECREASE:
                glStencilFail = GL_DECR;                
                break;
            case graphics::STENCIL_FUNCTIONS::DECREASE_WRAP:
                glStencilFail = GL_DECR_WRAP;                
                break;
            case graphics::STENCIL_FUNCTIONS::INVERT:
                glStencilFail = GL_INVERT;                
                break;
            default:
                glStencilFail = GL_KEEP;                
                break;
        }

        switch (depthFail)
        {
            case graphics::STENCIL_FUNCTIONS::KEEP:
                glDepthFail = GL_KEEP;
                break;
            case graphics::STENCIL_FUNCTIONS::ZERO:
                glDepthFail = GL_ZERO;                        
                break;
            case graphics::STENCIL_FUNCTIONS::REPLACE:
                glDepthFail = GL_REPLACE;                
                break;
            case graphics::STENCIL_FUNCTIONS::INCREASE:
                glDepthFail = GL_INCR;                
                break;
            case graphics::STENCIL_FUNCTIONS::INCREASE_WRAP:
                glDepthFail = GL_INCR_WRAP;                
                break;
            case graphics::STENCIL_FUNCTIONS::DECREASE:
                glDepthFail = GL_DECR;                
                break;
            case graphics::STENCIL_FUNCTIONS::DECREASE_WRAP:
                glDepthFail = GL_DECR_WRAP;                
                break;
            case graphics::STENCIL_FUNCTIONS::INVERT:
                glDepthFail = GL_INVERT;                
                break;
            default:
                glDepthFail = GL_KEEP;                
                break;
        }

        switch (pass)
        {
            case graphics::STENCIL_FUNCTIONS::KEEP:
                glPass = GL_KEEP;
                break;
            case graphics::STENCIL_FUNCTIONS::ZERO:
                glPass = GL_ZERO;                        
                break;
            case graphics::STENCIL_FUNCTIONS::REPLACE:
                glPass = GL_REPLACE;                
                break;
            case graphics::STENCIL_FUNCTIONS::INCREASE:
                glPass = GL_INCR;                
                break;
            case graphics::STENCIL_FUNCTIONS::INCREASE_WRAP:
                glPass = GL_INCR_WRAP;                
                break;
            case graphics::STENCIL_FUNCTIONS::DECREASE:
                glPass = GL_DECR;                
                break;
            case graphics::STENCIL_FUNCTIONS::DECREASE_WRAP:
                glPass = GL_DECR_WRAP;                
                break;
            case graphics::STENCIL_FUNCTIONS::INVERT:
                glPass = GL_INVERT;                
                break;
            default:
                glPass = GL_KEEP;                
                break;
        }

        glStencilOp(glStencilFail, glDepthFail, glPass);
    }

    void RHC_OpenGL::EnableFaceCulling()
    {
        glEnable(GL_CULL_FACE);
    }

    void RHC_OpenGL::DisableFaceCulling()
    {
        glDisable(GL_CULL_FACE);
    }

    void RHC_OpenGL::SetCulledFace(graphics::CULL_MODES face)
    {
        switch(face)
        {
            case graphics::CULL_MODES::FRONT_FACE:
                glCullFace(GL_FRONT);
                break;
            case graphics::CULL_MODES::BACK_FACE:
                glCullFace(GL_BACK);
                break;
            case graphics::CULL_MODES::FRONT_AND_BACK_FACE:
                glCullFace(GL_FRONT_AND_BACK);
                break;
            default:
                glCullFace(GL_FRONT);
                break;
        }
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