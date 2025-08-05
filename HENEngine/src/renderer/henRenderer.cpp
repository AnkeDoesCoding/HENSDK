#include "renderer/henRenderer.h"

// YES THIS RENDERER IS A FUCKING MESS BUT I AM GRADUALLY ABSTRACING SHIT UNTIL THIS BECOMES A PROPER RENDERER

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb/include/stb_image.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "graphics/henGraphics.h"
#include "core/henTimer.h"
#include "input/henInput.h"
#include "src/renderer/henRHC_OpenGL.h"
#include "scene/henScene.h"
#include "tools/henConsole.h"

namespace hen::renderer
{   
    static std::unique_ptr<RHC> CurrentRHC;

    scene::actors::Camera Camera(glm::vec3(0.0f,0.0f,0.0f));
    bool Initialised = false;
    BACKEND CurrentBackend = BACKEND::OPENGL;

    float vertices[] = 
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = 
    {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -7.0f), 
        glm::vec3(-1.5f, -2.2f, -3.5f),  
        glm::vec3(-3.8f, -2.0f, -5.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f),
        glm::vec3( 2.5f,  2.2f, -0.5f), 
        glm::vec3( 3.2f,  3.0f, -8.0f), 
        glm::vec3(-2.7f, -3.2f, -9.5f),  
        glm::vec3( 3.8f, -4.0f, -9.0f),  
        glm::vec3(-4.9f, -3.4f, -1.0f),  
        glm::vec3(-3.7f,  1.0f, -6.5f),  
        glm::vec3( 1.8f, -1.0f, -5.0f),  
        glm::vec3(-1.3f,  5.0f, -7.2f), 
        glm::vec3( 0.5f,  5.0f, -2.0f), 
        glm::vec3(-2.3f,  0.0f, -3.0f)
    };

    std::unique_ptr<graphics::VertexBuffer> VB;

    unsigned int VAO;
    unsigned int Texture;

    int ImageWidth, ImageHeight, Channels;

    std::unique_ptr<graphics::Shader> CubeShader;

    glm::mat4 model = glm::mat4(1.0f); 
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    
    void Initialise(SDL_Window* window)
    {
        Timer timer;

        HEN_ASSERT(window != nullptr, "Window is nullptr");

        switch(CurrentBackend)
        {
            case BACKEND::NONE:
                CurrentRHC = nullptr;
                console::Post("[hen::Renderer] Couldn't create RHC", console::LOGLEVEL::ERROR);
                break;
            case BACKEND::OPENGL:
                CurrentRHC = std::make_unique<RHC_OpenGL>(window);
                GetRHC() = CurrentRHC.get();    
                CurrentRHC->Initialise();
                break;
            default:
                break;
        }

        CubeShader.reset(graphics::Shader::Create(ENGINE_RESOURCE_PATH "shaders/GLSL/TriangleVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/TriangleFS.glsl"));
        
        CubeShader->Activate();

        VB.reset(graphics::VertexBuffer::Create(sizeof(vertices), vertices));

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

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
            console::Post("couldnt load texture", console::LOGLEVEL::WARNING);
        }        

        CubeShader->SetVal("ourTexture", 0);

        stbi_image_free(ImageData);
   
        Initialised = true;

        console::Post("[hen::renderer] Initialised in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void Run()
    {
        CurrentRHC->EnableDepth();
        CurrentRHC->Clear();
        
        CubeShader->Run();

        int windowWidth, windowHeight;
        SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

        projection = glm::perspective(glm::radians(Camera.FOV), (float)windowWidth / (float)windowHeight, 0.01f, 1000.0f);

        CubeShader->SetMat4("model", model);
        CubeShader->SetMat4("view", view);
        CubeShader->SetMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glBindVertexArray(VAO);
        for(unsigned int i = 0; i < 20; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 12.0f * i; 
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            CubeShader->SetMat4("model", model);
        
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        CurrentRHC->Present();
        CurrentRHC->DisableDepth();
    }

    void Update(float deltaTime)
    {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

        view = Camera.GetViewMatrix();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        Camera.SetDirty();
    }
}