#include "renderer/henRenderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb/include/stb_image.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "tools/henConsole.h"
#include "graphics/henShader.h"

hen::RenderHardwareContext* RHC;

float vertices[] = {
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
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
    bool Initialised = false;

    void Initialise()
    {

        RHC = GetRHC();    
        
        RHC->Initialise();

        console::Post("[hen::renderer] Initialised");

        TriangleShader.Activate();

        glGenBuffers(1, &VBO);  
        glBindBuffer(GL_ARRAY_BUFFER, VBO);  
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

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
        RHC->ClearSwapChain();
        
        TriangleShader.Run();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)(SDL_GetTicks() / 1000.0), glm::vec3(0.0f, 0.0f, 1.0f));

        TriangleShader.SetMat4("transform", trans);

        RHC->SwapSwapChain();
    }
}