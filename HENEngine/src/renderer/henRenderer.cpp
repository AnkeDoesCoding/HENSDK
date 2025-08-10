#include "renderer/henRenderer.h"

// YES THIS RENDERER IS A FUCKING MESS BUT I AM GRADUALLY ABSTRACING SHIT UNTIL THIS BECOMES A PROPER RENDERER


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "graphics/henGraphics.h"
#include "core/henArguments.h"
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

    std::unique_ptr <graphics::VertexArray> LampVA;
    std::shared_ptr <graphics::VertexBuffer> LampVB;
    std::shared_ptr <graphics::IndexBuffer> LampIB;

    std::vector<unsigned int> LampIndices;

    unsigned int VAO;
    unsigned int Texture;

    std::unique_ptr<graphics::Shader> CubeShader;
    std::unique_ptr<graphics::Shader> LampShader;

    glm::mat4 Model = glm::mat4(1.0f); 
    glm::mat4 Projection = glm::mat4(1.0f);
    
    glm::vec3 LightPos(0.0f, 1.0f, 1.0f);
    
    constexpr float PI = 3.14159265358979323846f;

    // some advanced maths shit Chat GPT helped me make
    static std::vector<float> GenerateSphereVertices(float radius, unsigned int sectorCount, unsigned int stackCount)
    {
        std::vector<float> vertices;
        float x, y, z, xy;
        float sectorStep = 2 * PI / sectorCount;
        float stackStep  = PI / stackCount;
        float sectorAngle, stackAngle;

        for (unsigned int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            y  = radius * sinf(stackAngle);

            for (unsigned int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;
                x = xy * cosf(sectorAngle);
                z = xy * sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }
        return vertices;
    }

    static std::vector<unsigned int> GenerateSphereIndices(unsigned int sectorCount, unsigned int stackCount)
    {
        std::vector<unsigned int> indices;
        unsigned int k1, k2;
        for (unsigned int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);
            k2 = k1 + sectorCount + 1;

            for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (i != (stackCount - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
        return indices;
    }
    
    void Initialise(SDL_Window* window)
    {
        Timer timer;

        HEN_ASSERT(window != nullptr, "Window is nullptr");

        if(arguments::HasArgument("vulkan"))
        {
            CurrentBackend = BACKEND::VULKAN;
        }

        switch(CurrentBackend)
        {
            case BACKEND::NONE:
                CurrentRHC = nullptr;
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                break;
            case BACKEND::OPENGL:
                CurrentRHC = std::make_unique<RHC_OpenGL>(window);
                GetRHC() = CurrentRHC.get();    
                CurrentRHC->Initialise();
                break;
            case BACKEND::VULKAN:
                CurrentRHC = nullptr; // hehe, set that mf to nullptr as a fuck you
                console::Log("[hen::renderer] BACKEND::VULKAN isn't supported, yet"); // PLANNED VULKAN SUPPORT !!!?!?!?!?!?!
            default:
                CurrentRHC = nullptr;
                break;
        }
        
        HEN_ASSERT(CurrentRHC != nullptr, "[hen::renderer] RHC is nullptr");

        CubeShader = graphics::Shader::Create(ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderFS.glsl");
        LampShader = graphics::Shader::Create(ENGINE_RESOURCE_PATH "shaders/GLSL/LampShaderVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/LampShaderFS.glsl");
        
        VB = graphics::VertexBuffer::Create(sizeof(vertices), vertices);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        auto lampVertices = GenerateSphereVertices(0.2f, 24, 16); // radius, sectors, stacks
        LampIndices = GenerateSphereIndices(24, 16);

        LampVA = graphics::VertexArray::Create();

        LampVB = graphics::VertexBuffer::Create(lampVertices.size() * sizeof(float), lampVertices.data());
        graphics::BufferLayout layout = {{graphics::SHADER_PRIMITIVES::FLOAT3, "aPos"}};
        LampVB->SetLayout(layout);
        LampVA->AddVertexBuffer(LampVB);

        LampIB = graphics::IndexBuffer::Create((uint32_t)LampIndices.size(), LampIndices.data());
        LampVA->SetIndexBuffer(LampIB);    

        // Texture = helpers::LoadTexture(ENGINE_RESOURCE_PATH "textures/container.jpg");

        // CubeShader->SetVal("Texture", 0);

        Initialised = true;

        console::Log("[hen::renderer] Initialised in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void Run()
    {
        CurrentRHC->Clear();

        CurrentRHC->EnableDepth();

        int windowWidth, windowHeight;
        SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

        Projection = glm::perspective(glm::radians(Camera.FOV), (float)windowWidth / (float)windowHeight, 0.01f, 1000.0f);

        CubeShader->Run();
 
        CubeShader->SetVec3("viewPos", Camera.Position);
        CubeShader->SetMat4("model", Model);
        CubeShader->SetMat4("view", Camera.GetViewMatrix());
        CubeShader->SetMat4("projection", Projection);

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

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        CubeShader->SetMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        LampShader->Run();

        LampShader->SetMat4("projection", Projection);
        LampShader->SetMat4("view", Camera.GetViewMatrix());
        model = glm::mat4(1.0f);
        model = glm::translate(model, LightPos);
        model = glm::scale(model, glm::vec3(0.5f));
        LampShader->SetMat4("model", model);

        LampVA->Bind();
        glDrawElements(GL_TRIANGLES, (GLsizei)LampIndices.size(), GL_UNSIGNED_INT, nullptr);

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