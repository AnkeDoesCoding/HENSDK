#include "renderer/henRenderer.h"

// YES THIS RENDERER IS A FUCKING MESS BUT I AM GRADUALLY ABSTRACING SHIT UNTIL THIS BECOMES A PROPER RENDERER

#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>

#include "graphics/henGraphics.h"
#include "core/henArguments.h"
#include "core/henTimer.h"
#include "core/henCVar.h"
#include "src/renderer/henRHC_OpenGL.h"
#include "tools/henConsole.h"
#include "ui/henUI.h"

namespace hen::renderer
{   
    static std::unique_ptr<RHC> CurrentRHC;
    static std::unique_ptr<ShaderManager> CurrentShaderManager;
    static ShaderHandle PrimitiveShader;

    static float CubeVertices[] =
    {
        // vertices           // normals
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,-1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f,-1.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f,-1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,-1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,-1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,-1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f,-1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f,-1.0f, 0.0f
    };

    static unsigned int CubeIndices[] = 
    {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        8, 9,10,  10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    static float SphereVertices[] =
    {
        // vertices                // normals
         0.0f,    1.0f,    0.0f,    0.0f,    1.0f,    0.0f,
         0.0f,    0.707f,  0.707f,  0.0f,    0.707f,  0.707f,
         0.5f,    0.707f,  0.5f,    0.5f,    0.707f,  0.5f,
         0.707f,  0.707f,  0.0f,    0.707f,  0.707f,  0.0f,
         0.5f,    0.707f, -0.5f,    0.5f,    0.707f, -0.5f,
         0.0f,    0.707f, -0.707f,  0.0f,    0.707f, -0.707f,
        -0.5f,    0.707f, -0.5f,   -0.5f,    0.707f, -0.5f,
        -0.707f,  0.707f,  0.0f,   -0.707f,  0.707f,  0.0f,
        -0.5f,    0.707f,  0.5f,   -0.5f,    0.707f,  0.5f,
         0.0f,    0.0f,    1.0f,    0.0f,    0.0f,    1.0f,
         0.707f,  0.0f,    0.707f,  0.707f,  0.0f,    0.707f,
         1.0f,    0.0f,    0.0f,    1.0f,    0.0f,    0.0f,
         0.707f,  0.0f,   -0.707f,  0.707f,  0.0f,   -0.707f,
         0.0f,    0.0f,   -1.0f,    0.0f,    0.0f,   -1.0f,
        -0.707f,  0.0f,   -0.707f, -0.707f,  0.0f,   -0.707f,
        -1.0f,    0.0f,    0.0f,   -1.0f,    0.0f,    0.0f,
        -0.707f,  0.0f,    0.707f, -0.707f,  0.0f,    0.707f,
         0.0f,   -0.707f,  0.707f,  0.0f,   -0.707f,  0.707f,
         0.5f,   -0.707f,  0.5f,    0.5f,   -0.707f,  0.5f,
         0.707f, -0.707f,  0.0f,    0.707f, -0.707f,  0.0f,
         0.5f,   -0.707f, -0.5f,    0.5f,   -0.707f, -0.5f,
         0.0f,   -0.707f, -0.707f,  0.0f,   -0.707f, -0.707f,
        -0.5f,   -0.707f, -0.5f,   -0.5f,   -0.707f, -0.5f,
        -0.707f, -0.707f,  0.0f,   -0.707f, -0.707f,  0.0f,
        -0.5f,   -0.707f,  0.5f,   -0.5f,   -0.707f,  0.5f,
         0.0f,   -1.0f,    0.0f,    0.0f,   -1.0f,    0.0f
    };


    static unsigned int SphereIndices[] = 
    {
        0,1,2, 0,2,3, 0,3,4, 0,4,5,
        0,5,6, 0,6,7, 0,7,8, 0,8,1,
        1,9,2, 2,9,10, 2,10,3, 3,10,11,
        3,11,4, 4,11,12, 4,12,5, 5,12,13,
        5,13,6, 6,13,14, 6,14,7, 7,14,15,
        7,15,8, 8,15,16, 8,16,1, 1,16,9,
        9,17,10, 10,17,18, 10,18,11, 11,18,19,
        11,19,12, 12,19,20, 12,20,13, 13,20,21,
        13,21,14, 14,21,22, 14,22,15, 15,22,23,
        15,23,16, 16,23,24, 16,24,9, 9,24,17,
        25,17,18, 25,18,19, 25,19,20, 25,20,21,
        25,21,22, 25,22,23, 25,23,24, 25,24,17
    };

    cvar::CVar cvar_VSync("r_vsync", false, cvar::FLAGS_ARCHIVE, []()
    {
        cvar_VSync.GetBool() ? CurrentRHC->EnableVSync() : CurrentRHC->DisableVSync();
    });  

    cvar::CVar cvar_FOV("r_fov", Camera.FOV, cvar::FLAGS_ARCHIVE, []()
    {
        Camera.FOV = cvar_FOV.GetFloat();
    });

    bool Initialised = false;
    BACKEND CurrentBackend = BACKEND::OPENGL;
    level::CameraComponent Camera(90.0f, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0, -90.0f, 0.0f));

    float vertices[] = 
    {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    std::unique_ptr <graphics::VertexBuffer> VB;

    unsigned int VAO;
    graphics::Texture2D DiffuseTexture;
    graphics::Texture2D SpecularTexture;

    graphics::Shader CubeShader;

    glm::mat4 Projection = glm::mat4(1.0f);
    
    glm::vec3 LightPos(0.0f, 1.0f, 1.0f);
    
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
                console::Log("[hen::renderer] BACKEND::????? how the fuck did we get here?");
                break;
        }
        
        HEN_ASSERT(CurrentRHC != nullptr, "[hen::renderer] RHC is nullptr");

        CurrentShaderManager = std::make_unique<ShaderManager>();
        GetShaderManager() = CurrentShaderManager.get();

        PrimitiveShader = CurrentShaderManager->CreateOrGet(ENGINE_RESOURCE_PATH "shaders/GLSL/PrimitiveShaderVS.glsl",ENGINE_RESOURCE_PATH "shaders/GLSL/PrimitiveShaderFS.glsl");

        CubeShader.Create(ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderFS.glsl");

        VB = graphics::VertexBuffer::Create(sizeof(vertices), vertices);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2); 

        DiffuseTexture.Load(ENGINE_RESOURCE_PATH "textures/container2.png");
        SpecularTexture.Load(ENGINE_RESOURCE_PATH "textures/container2_specular.png");

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

        glm::mat4 model = glm::mat4(1.0f);

        CubeShader.Bind();
 
        CubeShader.SetVec3("viewPos", Camera.Position);
        CubeShader.SetMat4("model", model);
        CubeShader.SetMat4("view", Camera.GetViewMatrix());
        CubeShader.SetMat4("projection", Projection);

        CubeShader.SetVal("material.diffuse", 0);
        CubeShader.SetVal("material.specular", 1);
        CubeShader.SetVal("material.shininess", 32.0f);
        CubeShader.SetVec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
        CubeShader.SetVec3("light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
        CubeShader.SetVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f)); 
        CubeShader.SetVec3("light.position", LightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DiffuseTexture.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SpecularTexture.ID);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        CubeShader.UnBind();

        RenderPrimitive(PRIMITIVES::SPHERE, LightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f), glm::vec3(1.0f));

        Camera.SetDirty(level::GetActiveLevel()->Up);

        RenderLevel();

        CurrentRHC->DisableDepth();

        ui::GetIMGUIManager()->BeginFrame();

        ui::GetIMGUIManager()->EndFrame();

        CurrentRHC->Present();
    }

    void Update(float deltaTime)
    {

    }

    void RenderPrimitive(PRIMITIVES primitve, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 colour)
    {
        static std::unique_ptr <graphics::VertexArray> cubeVertexArray;
        static std::unique_ptr <graphics::VertexArray> sphereVertexArray;

        static std::shared_ptr<graphics::VertexBuffer> cubeVB = graphics::VertexBuffer::Create(sizeof(CubeVertices), CubeVertices);
        static std::shared_ptr<graphics::IndexBuffer> cubeIB = graphics::IndexBuffer::Create(sizeof(CubeIndices), CubeIndices);

        static std::shared_ptr<graphics::VertexBuffer> sphereVB = graphics::VertexBuffer::Create(sizeof(SphereVertices), SphereVertices);
        static std::shared_ptr<graphics::IndexBuffer> sphereIB = graphics::IndexBuffer::Create(sizeof(SphereIndices), SphereIndices);

        static graphics::BufferLayout layout = 
        {
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aPos"},
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aNormal"}
        };

        static bool initialised = false;
        
        if (!initialised)
        {
            cubeVertexArray = graphics::VertexArray::Create();
            cubeVB->SetLayout(layout);
            cubeVertexArray->AddVertexBuffer(cubeVB);
            cubeVertexArray->SetIndexBuffer(cubeIB);

            sphereVertexArray = graphics::VertexArray::Create();
            sphereVB->SetLayout(layout);
            sphereVertexArray->AddVertexBuffer(sphereVB);
            sphereVertexArray->SetIndexBuffer(sphereIB);

            initialised = true;
        }

        glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotation));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * rotationMatrix;
        model = glm::scale(model, scale);


        auto* shader = CurrentShaderManager->Get(PrimitiveShader);

        shader->Bind();

        shader->SetVec3("colour", colour);
        shader->SetMat4("projection", Projection);
        shader->SetMat4("view", Camera.GetViewMatrix());
        shader->SetMat4("model", model);

        switch(primitve)
        {
            case PRIMITIVES::CUBE:
            {
                cubeVertexArray->Bind();
                glDrawElements(GL_TRIANGLES, (GLsizei)cubeVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
                cubeVertexArray->UnBind();
                break;
            }
            case PRIMITIVES::SPHERE:
            {
                sphereVertexArray->Bind();
                glDrawElements(GL_TRIANGLES, (GLsizei)sphereVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
                sphereVertexArray->UnBind();
                break;
            }
            default:
                break;
        }

        shader->UnBind();
    }

    void RenderLevel()
    {
        if (auto level = level::GetActiveLevel())
        {
            auto view = level->GetView<level::TransformComponent, level::MeshComponent>();

            for (auto entity : view)
            {
                auto& transformComp = entity.GetComponent<hen::level::TransformComponent>();
                auto& meshComp = entity.GetComponent<hen::level::MeshComponent>();
                
                if (level::GetActiveLevel() && entity.HasComponent<hen::level::MaterialComponent>())
                {
                    auto& materialComp = entity.GetComponent<hen::level::MaterialComponent>();

                    if (materialComp.DiffuseTexture.ID != 0 && materialComp.SpecularTexture.ID != 0)
                    {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, materialComp.DiffuseTexture.ID);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, materialComp.SpecularTexture.ID);
                    }
                }

                auto* shader = CurrentShaderManager->Get(PrimitiveShader);

                shader->Bind();

                shader->SetVec3("colour", glm::vec3(1.0f));
                shader->SetMat4("projection", Projection);
                shader->SetMat4("view", Camera.GetViewMatrix());
                shader->SetMat4("model", transformComp.Transform);

                if (meshComp.VertexArray)
                {
                    meshComp.VertexArray->Bind();
                    glDrawElements(GL_TRIANGLES, (GLsizei)meshComp.IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
                    meshComp.VertexArray->UnBind();
                }
                
                shader->UnBind();
                
            }
        }
    }
}