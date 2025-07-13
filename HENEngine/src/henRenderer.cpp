#include "renderer/henRenderer.h"

#include "vendor/glad/include/glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb/include/stb_image.h"

#include "tools/henConsole.h"
#include "graphics/henShader.h"

float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

 unsigned int indices[] = {  
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int Texture;

int ImageWidth, ImageHeight, Channels;

hen::graphics::Shader TriangleShader(ENGINE_RESOURCE_PATH "shaders/GLSL/TriangleVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/TriangleFS.glsl");

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

        TriangleShader.Activate();

        glGenBuffers(1, &VBO);  
        glBindBuffer(GL_ARRAY_BUFFER, VBO);  
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);

         // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned char* ImageData = stbi_load(ENGINE_RESOURCE_PATH "textures/container.jpg", &ImageWidth, &ImageHeight, &Channels, 0);

        if(ImageData != nullptr)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImageWidth, ImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            console::Post("couldnt load texture", console::Level::Warning);
        }        

        TriangleShader.SetVal("ourTexture", 0);

        stbi_image_free(ImageData);
   
        Initialised = true;
    }

    void Run()
    {   
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        TriangleShader.Run();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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