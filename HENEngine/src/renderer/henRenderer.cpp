#include "renderer/henRenderer.h"

// YES THIS RENDERER IS A FUCKING MESS BUT I AM GRADUALLY ABSTRACING SHIT UNTIL THIS BECOMES A PROPER RENDERER


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "graphics/henGraphics.h"
#include "core/henTimer.h"
#include "input/henInput.h"
#include "src/renderer/henRHC_OpenGL.h"
#include "scene/henScene.h"
#include "tools/henConsole.h"
#include "tools/henHelpers.h"

namespace hen::renderer
{   
    static std::unique_ptr<RHC> CurrentRHC;

    scene::actors::Camera Camera(glm::vec3(0.0f,0.0f, 3.0f));
    bool Initialised = false;
    BACKEND CurrentBackend = BACKEND::OPENGL;

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    std::unique_ptr <graphics::VertexBuffer> VB;

    unsigned int LampVAO;
    unsigned int VAO;
    unsigned int Texture;

    std::unique_ptr<graphics::Shader> CubeShader;
    std::unique_ptr<graphics::Shader> LampShader;

    glm::mat4 model = glm::mat4(1.0f); 
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    
    glm::vec3 LightPos(0.0f, 1.0f, 1.0f);
    
    void Initialise(SDL_Window* window)
    {
        Timer timer;

        HEN_ASSERT(window != nullptr, "Window is nullptr");

        switch(CurrentBackend)
        {
            case BACKEND::NONE:
                CurrentRHC = nullptr;
                console::Post("[hen::Renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                break;
            case BACKEND::OPENGL:
                CurrentRHC = std::make_unique<RHC_OpenGL>(window);
                GetRHC() = CurrentRHC.get();    
                CurrentRHC->Initialise();
                break;
            default:
                break;
        }

        CubeShader = graphics::Shader::Create(ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderFS.glsl");
        LampShader = graphics::Shader::Create(ENGINE_RESOURCE_PATH "shaders/GLSL/LampShaderVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/LampShaderFS.glsl");
        
        VB = graphics::VertexBuffer::Create(sizeof(vertices), vertices);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenVertexArrays(1, &LampVAO);
        glBindVertexArray(LampVAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        Texture = helpers::LoadTexture(ENGINE_RESOURCE_PATH "textures/container.jpg");

        // CubeShader->SetVal("Texture", 0);
   
        Initialised = true;

        console::Post("[hen::renderer] Initialised in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void Run()
    {
        CurrentRHC->Clear();

        CurrentRHC->EnableDepth();

        int windowWidth, windowHeight;
        SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

        view = Camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(Camera.FOV), (float)windowWidth / (float)windowHeight, 0.01f, 1000.0f);

        CubeShader->Run();
 
        CubeShader->SetVec3("viewPos", Camera.Position);
        CubeShader->SetMat4("model", model);
        CubeShader->SetMat4("view", view);
        CubeShader->SetMat4("projection", projection);

        CubeShader->SetVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        CubeShader->SetVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        CubeShader->SetVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        CubeShader->SetVal("material.shininess", 32.0f);
        CubeShader->SetVec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
        CubeShader->SetVec3("light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
        CubeShader->SetVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f)); 
        CubeShader->SetVec3("light.position", LightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glBindVertexArray(VAO);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        CubeShader->SetMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        LampShader->Run();

        LampShader->SetMat4("Projection", projection);
        LampShader->SetMat4("View", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, LightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        LampShader->SetMat4("Model", model);

        glBindVertexArray(LampVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        Camera.SetDirty();

        CurrentRHC->DisableDepth();

        //2D renderering goes here

        CurrentRHC->Present();
    }

    void Update(float deltaTime)
    {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

        
    }
}