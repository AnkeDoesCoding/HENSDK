#include "renderer/henRenderer.h"

#include "thirdparty/glad/include/glad/glad.h"

#include "tools/henConsole.h"

float vertices[] =
{
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f    
};

unsigned int VBO;
unsigned int VAO;

unsigned int VertexShader;
unsigned int FragShader;
unsigned int ShaderProgram;

const char *VertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *FragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

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

        glGenBuffers(1, &VBO);  
        glBindBuffer(GL_ARRAY_BUFFER, VBO);  
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        VertexShader = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
        glCompileShader(VertexShader);

        FragShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(FragShader, 1, &FragmentShaderSource, NULL);
        glCompileShader(FragShader);

        ShaderProgram = glCreateProgram();

        glAttachShader(ShaderProgram, VertexShader);
        glAttachShader(ShaderProgram, FragShader);
        glLinkProgram(ShaderProgram);

        glUseProgram(ShaderProgram);

        glDeleteShader(VertexShader);
        glDeleteShader(FragShader);  

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);  
        

        Initialised = true;
    }

    void Run()
    {   
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(ShaderProgram);
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