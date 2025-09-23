#include "renderer/henRenderer.h"

// YES THIS RENDERER IS A FUCKING MESS BUT I AM GRADUALLY ABSTRACING SHIT UNTIL THIS BECOMES A PROPER RENDERER

#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>
// shittiest fucking imgui implementation, i nuked almost everything in that folder to get this shit working
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/backends/imgui_impl_sdl3.h"
#include "vendor/imgui/backends/imgui_impl_opengl3.h"

#include "graphics/henGraphics.h"
#include "core/henArguments.h"
#include "core/henTimer.h"
#include "core/henCVar.h"
#include "input/henInput.h"
#include "src/renderer/henRHC_OpenGL.h"
#include "scene/henScene.h"
#include "tools/henConsole.h"
#include "tools/henHelpers.h"

namespace hen::renderer
{   
    static std::unique_ptr<RHC> CurrentRHC;
    static std::unique_ptr<graphics::Shader> PrimitiveShader;

    static float CubeVertices[] =
    {
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f, -0.5f,
    };

    static unsigned int CubeIndices[] = 
    {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        8, 9,10, 10,11, 8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    static float SphereVertices[] = 
    {
        0.0f, 1.0f, 0.0f,
        0.0f, 0.707f, 0.707f,
        0.5f, 0.707f, 0.5f,
        0.707f, 0.707f, 0.0f,
        0.5f, 0.707f, -0.5f,
        0.0f, 0.707f, -0.707f,
        -0.5f, 0.707f, -0.5f,
        -0.707f, 0.707f, 0.0f,
        -0.5f, 0.707f, 0.5f,
        0.0f, 0.0f, 1.0f,
        0.707f, 0.0f, 0.707f,
        1.0f, 0.0f, 0.0f,
        0.707f, 0.0f, -0.707f,
        0.0f, 0.0f, -1.0f,
        -0.707f, 0.0f, -0.707f,
        -1.0f, 0.0f, 0.0f,
        -0.707f, 0.0f, 0.707f,
        0.0f, -0.707f, 0.707f,
        0.5f, -0.707f, 0.5f,
        0.707f, -0.707f, 0.0f,
        0.5f, -0.707f, -0.5f,
        0.0f, -0.707f, -0.707f,
        -0.5f, -0.707f, -0.5f,
        -0.707f, -0.707f, 0.0f,
        -0.5f, -0.707f, 0.5f,
        0.0f, -1.0f, 0.0f
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

    scene::actors::Camera Camera(glm::vec3(0.0f,0.0f, 3.0f));
    bool Initialised = false;
    BACKEND CurrentBackend = BACKEND::OPENGL;

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
    unsigned int DiffuseTexture;
    unsigned int SpecularTexture;

    unsigned int Counter;
    std::string FPS;

    std::unique_ptr<graphics::Shader> CubeShader;

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

        PrimitiveShader = graphics::Shader::Create(ENGINE_RESOURCE_PATH "shaders/GLSL/PrimitiveShaderVS.glsl",ENGINE_RESOURCE_PATH "shaders/GLSL/PrimitiveShaderFS.glsl");

        CubeShader = graphics::Shader::Create(ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderFS.glsl");

        VB = graphics::VertexBuffer::Create(sizeof(vertices), vertices);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2); 

        DiffuseTexture = helpers::LoadTexture(ENGINE_RESOURCE_PATH "textures/container2.png");
        SpecularTexture = helpers::LoadTexture(ENGINE_RESOURCE_PATH "textures/container2_specular.png");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsClassic();
        ImGui_ImplSDL3_InitForOpenGL(window, SDL_GL_GetCurrentContext());
        ImGui_ImplOpenGL3_Init("#version 460");

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

        CubeShader->Bind();
 
        CubeShader->SetVec3("viewPos", Camera.Position);
        CubeShader->SetMat4("model", model);
        CubeShader->SetMat4("view", Camera.GetViewMatrix());
        CubeShader->SetMat4("projection", Projection);

        CubeShader->SetVal("material.diffuse", 0);
        CubeShader->SetVal("material.specular", 1);
        CubeShader->SetVal("material.shininess", 32.0f);
        CubeShader->SetVec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
        CubeShader->SetVec3("light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
        CubeShader->SetVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f)); 
        CubeShader->SetVec3("light.position", LightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SpecularTexture);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        CubeShader->SetMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        CubeShader->UnBind();

        // RenderPrimitive(PRIMITIVES::SPHERE, LightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f), glm::vec3(1.0f));

        RenderLevel();

        Camera.SetDirty();

        CurrentRHC->DisableDepth();

        //2D renderering goes here

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Info");

        if(ImGui::CollapsingHeader("Controls"))
        {
            ImGui::Text("W,A,S,D - move around");
            ImGui::Text("M - toggle mouse lock");
            ImGui::Text("Tilde - toggle console");
            ImGui::Text("Esc - shutdown application");
        }

        if(ImGui::CollapsingHeader("Stats"))
        {
            ImGui::Text("FPS:  %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("MS:  %.3f", 1000.0f / ImGui::GetIO().Framerate);

        }

        if(ImGui::CollapsingHeader("Camera"))
        {
            ImGui::Text("Speed:  %.1f", Camera.Speed);
            ImGui::Text("FOV:  %.0f", Camera.FOV);
            ImGui::Text("Yaw:  %.3f", Camera.Yaw);
            ImGui::Text("Pitch:  %.1f", Camera.Pitch);
            ImGui::Text("Position:  %.4f, %.4f, %.4f", Camera.Position.x, Camera.Position.y, Camera.Position.z);
        }
        ImGui::End();

        console::Draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        CurrentRHC->Present();
    }

    void Update(float deltaTime)
    {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

        Counter++;
        if(deltaTime >= 1.0 / 30.0)
        {
            FPS = std::to_string((1.0 / deltaTime) * Counter);
        }
    }

    void ProcessEvent(const SDL_Event& event)
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    void RenderPrimitive(PRIMITIVES primitve, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 colour)
    {
        static std::unique_ptr <graphics::VertexArray> vertexArray = graphics::VertexArray::Create();
        static std::shared_ptr <graphics::VertexBuffer> vertexBuffer;
        static std::shared_ptr <graphics::IndexBuffer> indexBuffer;

        switch(primitve)
        {
            case PRIMITIVES::CUBE:
                vertexBuffer = graphics::VertexBuffer::Create(sizeof(CubeVertices), CubeVertices);
                indexBuffer = graphics::IndexBuffer::Create(sizeof(CubeIndices), CubeIndices);
                break;
            case PRIMITIVES::SPHERE:
                vertexBuffer = graphics::VertexBuffer::Create(sizeof(SphereVertices), SphereVertices);
                indexBuffer = graphics::IndexBuffer::Create(sizeof(SphereIndices), SphereIndices);
                break;
            default:
                break;
        }
        

        graphics::BufferLayout layout = 
        {
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aPos"}
        };

        vertexBuffer->SetLayout(layout);

        vertexArray->AddVertexBuffer(vertexBuffer);
        vertexArray->SetIndexBuffer(indexBuffer);
        
        glm::quat quat = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
        glm::mat4 rotationMatrix = glm::toMat4(quat);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * rotationMatrix;
        model = glm::scale(model, scale);

        PrimitiveShader->Bind();

        PrimitiveShader->SetVec3("colour", colour);

        PrimitiveShader->SetMat4("projection", Projection);
        PrimitiveShader->SetMat4("view", Camera.GetViewMatrix());
        PrimitiveShader->SetMat4("model", model);

        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, (GLsizei)indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
        vertexArray->UnBind();

        PrimitiveShader->UnBind();
    }

    void RenderLevel()
    {
        if (auto level = level::GetActiveLevel())
        {
            auto view = level->GetView<level::TransformComponent, level::MeshComponent>();

            for (auto entity : view)
            {
                auto& transform = entity.GetComponent<hen::level::TransformComponent>();
                auto& mesh      = entity.GetComponent<hen::level::MeshComponent>();


                glm::vec3 position = transform.GetPosition();
                glm::vec3 rotation = transform.GetRotation();
                glm::vec3 scale    = transform.GetScale();

                
                RenderPrimitive(PRIMITIVES::CUBE, position, rotation, scale, glm::vec3(1.0f));
            }
        }
    }
}