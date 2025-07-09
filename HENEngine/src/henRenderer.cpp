#include "renderer/henRenderer.h"

#include "thirdparty/glad/include/glad/glad.h"

#include "tools/henConsole.h"

namespace hen::renderer
{   
    SDL_Window* g_Window;
    bool Initialised = false;
    
    void Initialise()
    {
        SDL_GLContext context = SDL_GL_CreateContext(g_Window);

        if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            Initialised = false;
            console::Post("[hen::renderer] Failed to initialised", console::Level::ExtremeError);
        }

        console::Post("[hen::renderer] Initialised");

        Initialised = true;
    }

    void Run()
    {   
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(g_Window);
    }

    void SetWindow(SDL_Window* window)
    {
        g_Window = window;
    }
}