#include "renderer/henRenderer.h"

#include "vendor/glad/include/glad/glad.h"

#include "tools/henConsole.h"
#include "graphics/henShader.h"

float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
}; 

unsigned int VBO;
unsigned int VAO;

hen::graphics::Shader TriangleShader("../../HENEngine/res/shaders/GLSL/TriangleVS.glsl", "../../HENEngine/res/shaders/GLSL/TriangleFS.glsl");

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
            console::Post("[hen::renderer] FAILED TO INITAILISE", console::Level::ExtremeError);
        }

        console::Post("[hen::renderer] Initialised");

        glGenBuffers(1, &VBO);  
        glBindBuffer(GL_ARRAY_BUFFER, VBO);  
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(1);
        
        TriangleShader.Activate();

        Initialised = true;
    }

    void Run()
    {   
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        TriangleShader.Run();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(g_Window);
    }

    void SetWindow(SDL_Window* window)
    {
        g_Window = window;
    }

    void ResizeWindow()
    {
        int newWidth, newHeight;
        SDL_GetWindowSize(g_Window, &newWidth, &newHeight);

        console::Post(std::to_string(newWidth) + " : " + std::to_string(newHeight));

        glViewport(0, 0, newWidth, newHeight);

    }
}