#include "renderer/henRenderer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>
#include "vendor/glm/gtx/string_cast.hpp"

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
    static std::unique_ptr<TextureManager> CurrentTextureManager;

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

    glm::vec3 LightPos;
    
    void Initialise(SDL_Window* window)
    {
        Timer timer;

        HEN_ASSERT(window != nullptr, "Window is nullptr");

        if (arguments::HasArgument("vulkan"))
        {
            CurrentBackend = BACKEND::VULKAN;
        }

        switch (CurrentBackend)
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
        
        HEN_ASSERT(CurrentRHC != nullptr, "RHC is nullptr");

        CurrentShaderManager = std::make_unique<ShaderManager>();
        GetShaderManager() = CurrentShaderManager.get();

        CurrentTextureManager = std::make_unique<TextureManager>();
        GetTextureManager() = CurrentTextureManager.get();

        PrimitiveShader = CurrentShaderManager->Load(ENGINE_RESOURCE_PATH "shaders/GLSL/PrimitiveShaderVS.glsl",ENGINE_RESOURCE_PATH "shaders/GLSL/PrimitiveShaderFS.glsl");

        Initialised = true;

        console::Log("[hen::renderer] Initialised in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void Run()
    {
        CurrentRHC->Clear();

        CurrentRHC->EnableDepth();

        Camera.SetDirty(level::GetActiveLevel()->Up);

        RenderLevel();

        float radius = 100.0f;
        float speed = 1.0f;
            
        float time = (float)SDL_GetTicks() / 1000.0f; // cba to bring back Update(float deltaTime)
            
        LightPos.x = radius * cos(time * speed);
        LightPos.y = 5.0f;
        LightPos.z = radius * sin(time * speed);

        RenderPrimitive(graphics::PRIMITIVES::SPHERE, LightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f), glm::vec3(1.0f));

        CurrentRHC->DisableDepth();

        ui::GetIMGUIManager()->BeginFrame();

        ui::GetIMGUIManager()->EndFrame();

        CurrentRHC->Present();
    }

    void RenderPrimitive(graphics::PRIMITIVES primitve, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 colour)
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

        int windowWidth, windowHeight;
        SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

        shader->SetVec3("uColour", colour);
        shader->SetMat4("uProjection", glm::perspective(glm::radians(Camera.FOV), (float)windowWidth / (float)windowHeight, Camera.NearPlane, Camera.FarPlane));
        shader->SetMat4("uView", Camera.GetViewMatrix());
        shader->SetMat4("uModel", model);

        switch (primitve)
        {
            case graphics::PRIMITIVES::CUBE:
                cubeVertexArray->Bind();
                CurrentRHC->Draw(cubeVertexArray->GetIndexBuffer()->GetCount());
               
                break;
            case graphics::PRIMITIVES::SPHERE:
                sphereVertexArray->Bind();
                CurrentRHC->Draw(sphereVertexArray->GetIndexBuffer()->GetCount());
                break;
            default:
                break;
        }

        cubeVertexArray->UnBind();
        sphereVertexArray->UnBind();

        shader->UnBind();
    }

    void RenderLevel()
    {

        if (auto level = level::GetActiveLevel())
        {
            auto litView = level->GetView<level::TransformComponent, level::MeshComponent, level::MaterialComponent>();

            for (auto entity : litView)
            {
                auto& transformComp = entity.GetComponent<level::TransformComponent>();
                auto& meshComp = entity.GetComponent<level::MeshComponent>();
                auto& materialComp = entity.GetComponent<level::MaterialComponent>();

                auto* diffuse = CurrentTextureManager->Get(materialComp.DiffuseTexture);
                auto* specular = CurrentTextureManager->Get(materialComp.SpecularTexture);

                if (diffuse && specular)
                {
                    glBindTexture(GL_TEXTURE_2D, diffuse->ID);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, specular->ID);
                }
                
                if (materialComp.Shader.IsValid())
                {
                    auto* shader = CurrentShaderManager->Get(materialComp.Shader);

                    shader->Bind();

                    int windowWidth, windowHeight;
                    SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

                    shader->SetMat4("uProjection", glm::perspective(glm::radians(Camera.FOV), (float)windowWidth / (float)windowHeight, Camera.NearPlane, Camera.FarPlane));
                    shader->SetMat4("uView", Camera.GetViewMatrix());
                    shader->SetMat4("uModel", transformComp.Transform);
                    shader->SetVec3("uViewPos", Camera.Position);

                    shader->SetVal("uMaterial.Diffuse", 0);
                    shader->SetVal("uMaterial.Specular", 1);
                    shader->SetVal("uMaterial.Shininess", 32.0f);
                    shader->SetVec3("uLight.Ambient",  glm::vec3(0.05f));
                    shader->SetVec3("uLight.Diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
                    shader->SetVec3("uLight.Specular", glm::vec3(1.0f, 1.0f, 1.0f)); 
                    shader->SetVec3("uLight.Position", LightPos);

                    if (meshComp.VertexArray)
                    {
                        meshComp.VertexArray->Bind();
                        CurrentRHC->Draw(meshComp.IndexBuffer->GetCount());
                        meshComp.VertexArray->UnBind();
                    }

                    shader->UnBind();
                }
                else
                {
                    console::Log("[hen::renderer] Entity " + entity.GetComponent<hen::level::NameComponent>().Name + " doesn't have a shader", console::LOGLEVEL::WARNING);
                }       
            }
        }
    }
}