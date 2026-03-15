#include "renderer/henRenderer.h"

#include "graphics/henGraphics.h"
#include "core/henArguments.h"
#include "core/henTimer.h"
#include "core/henCVar.h"
#include "core/henMath.h"
#include "src/renderer/henRHC_OpenGL.h"
#include "tools/henConsole.h"
#include "ui/henUI.h"

#include <glad/include/glad.h>

namespace hen::renderer
{   
    static std::unique_ptr<RHC> CurrentRHC;
    static std::unique_ptr<ShaderManager> CurrentShaderManager;
    static std::unique_ptr<TextureManager> CurrentTextureManager;

    static graphics::UniformBuffer LevelLightsUB;

    static std::shared_ptr<graphics::VertexBuffer> PrimitiveCubeVB;
    static std::shared_ptr<graphics::IndexBuffer> PrimitiveCubeIB;  
    static std::shared_ptr<graphics::VertexBuffer> PrimitiveSphereVB;
    static std::shared_ptr<graphics::IndexBuffer> PrimitiveSphereIB;

    static ShaderHandle PrimitiveShader;
    static ShaderHandle SkyboxShader;

    struct alignas(16) ShaderDirLight 
    {
        math::Vec4 Colour;

        math::Vec3 Ambient;
        float Pad0;
        math::Vec3 Direction;
        float Pad1;
    };  

    struct alignas(16) ShaderPointLight
    {
        math::Vec4 Colour;

        math::Vec3 Ambient;
        float Pad0;
        math::Vec3 Position; 
        float Pad1;

        math::Vec4 Attenuation;
    };

    struct alignas(16) ShaderSpotLight 
    {
        math::Vec4 Colour;

        math::Vec3 Ambient;
        float Pad0;
        math::Vec3 Position;
        float Pad1;
        math::Vec3 Direction;
        float Pad2;

        math::Vec4 Angles;

        math::Vec4 Attenuation;
    };

    struct ShaderLights
    {
        ShaderPointLight PointLights[100];
        ShaderSpotLight SpotLights[100];

        ShaderDirLight DirLight;

        int NumberOfPointLights;
        int NumberOfSpotLights;

        int HasDirectionalLight; // has to be a fuckass int because of alignment shit
    };

    bool Initialised = false;
    BACKEND CurrentBackend = BACKEND::OPENGL;
    level::CameraComponent Camera(90.0f, math::Vec3(0.0f, 10.0f, 0.0f), math::Vec3(0.0f));

    cvar::CVar cvar_VSync("r_vsync", false, cvar::FLAGS_ARCHIVE, []()
    {
        cvar_VSync.GetBool() ? CurrentRHC->EnableVSync() : CurrentRHC->DisableVSync();
    });  

    cvar::CVar cvar_FOV("r_fov", Camera.FOV, cvar::FLAGS_ARCHIVE, []()
    {
        Camera.FOV = cvar_FOV.GetFloat();

    });

    cvar::CVar cvar_NearPlane("r_near_plane", Camera.NearPlane, cvar::FLAGS_ARCHIVE, []() 
    {
        Camera.NearPlane = cvar_NearPlane.GetFloat();
    });

    cvar::CVar cvar_FarPlane("r_far_plane", Camera.FarPlane, cvar::FLAGS_ARCHIVE, []() 
    {
        Camera.FarPlane = cvar_FarPlane.GetFloat();
    });

    cvar::CVar cvar_SkyboxScale("r_skybox_scale", 64.0f);

    void Initialise(SDL_Window* window)
    {
        Timer timer;

        if (arguments::HasArgument("vulkan"))
        {
            CurrentBackend = BACKEND::VULKAN;
        }

        switch (CurrentBackend)
        {
            case BACKEND::OPENGL:
                CurrentRHC = std::make_unique<RHC_OpenGL>(window);
                GetRHC() = CurrentRHC.get();    
                CurrentRHC->Initialise();
                break;
            case BACKEND::VULKAN: 
                CurrentRHC = nullptr; // hehe, set that mf to nullptr as a fuck you
                HEN_ERROR("[hen::renderer] BACKEND::VULKAN isn't supported, yet"); // PLANNED VULKAN SUPPORT !!!?!?!?!?!?!
            default:
                CurrentRHC = nullptr;
                HEN_ERROR("[hen::renderer] BACKEND::????? how the fuck did we get here?");
                break;
        }
        
        HEN_ASSERT(CurrentRHC != nullptr, "RHC is nullptr");

        CurrentRHC->EnableDepth();

        CurrentShaderManager = std::make_unique<ShaderManager>();
        GetShaderManager() = CurrentShaderManager.get();

        CurrentTextureManager = std::make_unique<TextureManager>();
        GetTextureManager() = CurrentTextureManager.get();

        PrimitiveShader = CurrentShaderManager->Load("res/engine/shaders/GLSL/PrimitiveShaderVS.glsl", "res/engine/shaders/GLSL/PrimitiveShaderFS.glsl");
        SkyboxShader = CurrentShaderManager->Load("res/engine/shaders/GLSL/SkyboxShaderVS.glsl", "res/engine/shaders/GLSL/SkyboxShaderFS.glsl");

        LevelLightsUB.Create(sizeof(ShaderLights), 1);

        PrimitiveCubeVB = graphics::VertexBuffer::Create(sizeof(level::cube::Vertices), level::cube::Vertices);
        PrimitiveCubeIB = graphics::IndexBuffer::Create(sizeof(level::cube::Indices), level::cube::Indices);
        PrimitiveSphereVB = graphics::VertexBuffer::Create(sizeof(level::sphere::Vertices), level::sphere::Vertices);
        PrimitiveSphereIB = graphics::IndexBuffer::Create(sizeof(level::sphere::Indices), level::sphere::Indices);

        Initialised = true;

        HEN_LOG("[hen::renderer] Initialised in " + std::to_string(static_cast<int>(std::round(timer.ElapsedMilliseconds()))) + " ms");
    }

    void Run()
    {
        CurrentRHC->Clear();

        if (auto level = level::GetActiveLevel())
        {
            Camera.SetDirty(level->Up);
        }

        PreRender();

        Render();

        ui::GetIMGUIManager()->BeginFrame();

        ui::GetIMGUIManager()->EndFrame();

        CurrentRHC->Present();
    }

    void PreRender()
    {
        if (auto level = level::GetActiveLevel())
        {
            ShaderLights data;
            data.NumberOfPointLights = 0;
            data.NumberOfSpotLights = 0;
            data.HasDirectionalLight = false;

            int pointLightIndex = 0;
            int spotLightIndex = 0;

            float linear;
            float quadratic; 

            auto lights = level->GetView<level::TransformComponent, level::LightComponent>();

            for (auto light : lights)
            {
                auto& transformComp = light.GetComponent<level::TransformComponent>();
                auto& lightComp = light.GetComponent<level::LightComponent>();

                switch (lightComp.Type)
                {
                    case level::LIGHT_TYPES::POINT:
                        if (pointLightIndex >= 100)
                        {
                            continue;
                        }
                    
                        linear = 4.5f / lightComp.Range;
                        quadratic = 75.f / (lightComp.Range * lightComp.Range);
                        
                        data.PointLights[pointLightIndex].Position = transformComp.LocalPosition;
                        data.PointLights[pointLightIndex].Colour = math::Vec4(lightComp.Colour, 0.0f);
                        data.PointLights[pointLightIndex].Ambient = lightComp.Ambient;
                        data.PointLights[pointLightIndex].Attenuation.x = 1.0f;
                        data.PointLights[pointLightIndex].Attenuation.y = linear;
                        data.PointLights[pointLightIndex].Attenuation.z = quadratic;
                        data.PointLights[pointLightIndex].Colour.w = lightComp.Intensity;
                        
                        RenderPrimitive(level::PRIMITIVE_TYPES::SPHERE, transformComp.LocalPosition, math::Vec3(0.0f), transformComp.LocalScale, lightComp.Colour);
                    
                        pointLightIndex++;
                        data.NumberOfPointLights++;
                        break;
                    case level::LIGHT_TYPES::SPOT:
                        if (spotLightIndex >= 100)
                        {
                            continue;
                        }
                    
                        linear = 4.5 / lightComp.Range;
                        quadratic = 75 / (lightComp.Range * lightComp.Range);
                        
                        data.SpotLights[spotLightIndex].Position = transformComp.LocalPosition;
                        data.SpotLights[spotLightIndex].Direction = transformComp.GetForwardVector(); 
                        data.SpotLights[spotLightIndex].Colour = math::Vec4(lightComp.Colour, 0.0f);
                        data.SpotLights[spotLightIndex].Ambient = lightComp.Ambient;
                        data.SpotLights[spotLightIndex].Attenuation.x = 1.0f;
                        data.SpotLights[spotLightIndex].Attenuation.y = linear;
                        data.SpotLights[spotLightIndex].Attenuation.z = quadratic;
                        data.SpotLights[spotLightIndex].Colour.w = lightComp.Intensity;
                        data.SpotLights[spotLightIndex].Angles.x = math::Cos(math::Radians(lightComp.InnerCutOff));
                        data.SpotLights[spotLightIndex].Angles.y = math::Cos(math::Radians(lightComp.OuterCutOff));
                    
                        RenderPrimitive(level::PRIMITIVE_TYPES::SPHERE, transformComp.LocalPosition, math::Vec3(0.0f), transformComp.LocalScale, lightComp.Colour);
                    
                        spotLightIndex++;
                        data.NumberOfSpotLights++;
                        break;
                    case level::LIGHT_TYPES::DIRECTIONAL:
                        if (data.HasDirectionalLight)
                        {
                            continue;
                        }
                    
                        data.DirLight.Direction = transformComp.GetForwardVector();
                        data.DirLight.Colour = math::Vec4(lightComp.Colour, 0.0f);
                        data.DirLight.Ambient = lightComp.Ambient;
                        data.DirLight.Colour.w = lightComp.Intensity;
                    
                        data.HasDirectionalLight = true;
                        break; 
                    default:
                        break;
                }
            }

            LevelLightsUB.SetData(&data, sizeof(ShaderLights));

            auto renderEntities = level->GetView<level::MeshComponent, level::MaterialComponent>();

            for (auto entity : renderEntities)
            {
                auto& meshComp = entity.GetComponent<level::MeshComponent>();
                auto& materialComp = entity.GetComponent<level::MaterialComponent>();

                if (meshComp.State == graphics::RESOURCE_STATES::READY_TO_UPLOAD)
                {
                    meshComp.CreateRenderData();
                }

                for (auto& submesh : meshComp.SubMeshes)
                {
                    if (submesh.DiffuseIndex < materialComp.DiffuseTextures.size())
                    {
                        if (auto* diffuse = CurrentTextureManager->Get(materialComp.DiffuseTextures[submesh.DiffuseIndex]))
                        {
                            if (diffuse->State == graphics::RESOURCE_STATES::READY_TO_UPLOAD)
                            {
                                diffuse->CreateRenderData();
                            }
                        }
                    }

                    if (submesh.SpecularIndex < materialComp.SpecularTextures.size())
                    {
                        if (auto *specular = CurrentTextureManager->Get(materialComp.SpecularTextures[submesh.SpecularIndex]))
                        {
                            if (specular->State == graphics::RESOURCE_STATES::READY_TO_UPLOAD)
                            {
                                specular->CreateRenderData();
                            }
                        }
                    }
                }
            }

            if (level->Skybox.Mesh.State == graphics::RESOURCE_STATES::READY_TO_UPLOAD)
            {
                level->Skybox.Mesh.CreateRenderData();
            }
        }
    }

    void Render()
    {
        if (auto level = level::GetActiveLevel())
        {   
            int windowWidth, windowHeight;
            SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

            if (level->Skybox.Mesh.State == graphics::RESOURCE_STATES::READY_TO_RENDER)
            {
                for (auto submesh : level->Skybox.Mesh.SubMeshes)
                {
                    auto* shader = CurrentShaderManager->Get(SkyboxShader);

                    shader->Bind();

                    math::Matrix4 model = math::Translate(math::Matrix4(1.0f), math::Vec3(0.0f));

                    math::Matrix4 view = math::LookAt(Camera.Position / cvar_SkyboxScale.GetFloat(), (Camera.Position + Camera.Front) / cvar_SkyboxScale.GetFloat(), math::Vec3(0.0f, 1.0f, 0.0f));

                    shader->SetMat4("uView", view);
                    shader->SetMat4("uModel", model);

                    shader->SetVec3("uLightDir", math::Vec3(0.5f, 1.0f, 0.5f));
                    shader->SetVec3("uLightColour", math::Vec3(1.0f, 1.0f, 1.0f));
                    shader->SetVal("uLightIntensity", 1.0f);

                    shader->SetMat4("uProjection", Camera.GetProjection(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));

                    level->Skybox.Mesh.VertexArray.Bind();
                    CurrentRHC->DrawElements(submesh.IndexCount, submesh.IndexStart);
                    level->Skybox.Mesh.VertexArray.UnBind();

                    shader->UnBind();
                }

                CurrentRHC->ClearDepth();
            }

            auto litEntities = level->GetView<level::TransformComponent, level::MeshComponent, level::MaterialComponent>();

            for (auto entity : litEntities)
            {
                auto& transformComp = entity.GetComponent<level::TransformComponent>();
                auto& meshComp = entity.GetComponent<level::MeshComponent>();
                auto& materialComp = entity.GetComponent<level::MaterialComponent>();

                auto* shader = CurrentShaderManager->Get(materialComp.Shader);
                shader->Bind();
            
                if (meshComp.State != graphics::RESOURCE_STATES::READY_TO_RENDER)
                {
                    continue;
                }

                meshComp.VertexArray.Bind();

                for (auto &submesh : meshComp.SubMeshes)
                {
                    if (submesh.DiffuseIndex < materialComp.DiffuseTextures.size())
                    {
                        if (auto *diffuse = CurrentTextureManager->Get(materialComp.DiffuseTextures[submesh.DiffuseIndex]))
                        {
                            shader->SetVal("uMaterial.HasDiffuse", 1);

                            if (diffuse->State == graphics::RESOURCE_STATES::READY_TO_RENDER)
                            {
                                glActiveTexture(GL_TEXTURE0);
                                glBindTexture(GL_TEXTURE_2D, diffuse->ID);
                            }
                        }
                        else
                        {
                            shader->SetVal("uMaterial.HasDiffuse", 0);
                        }
                    }

                    if (submesh.SpecularIndex < materialComp.SpecularTextures.size())
                    {
                        if (auto *specular = CurrentTextureManager->Get(materialComp.SpecularTextures[submesh.SpecularIndex]))
                        {
                            shader->SetVal("uMaterial.HasSpecular", 1);

                            if (specular->State == graphics::RESOURCE_STATES::READY_TO_UPLOAD)
                            {
                                glActiveTexture(GL_TEXTURE1);
                                glBindTexture(GL_TEXTURE_2D, specular->ID);
                            }
                        }
                        else
                        {
                            shader->SetVal("uMaterial.HasSpecular", 0);
                        }
                    }

                    shader->SetMat4("uProjection", Camera.GetProjection(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
                    shader->SetMat4("uView", Camera.GetViewMatrix());
                    shader->SetMat4("uModel", transformComp.GetWorldMatrix());
                    shader->SetVec3("uViewPos", Camera.Position);

                    shader->SetVal("uMaterial.Diffuse", 0);
                    shader->SetVal("uMaterial.Specular", 1);
                    shader->SetVal("uMaterial.Shininess", 32.0f);
                    shader->SetVec3("uMaterial.Colour", materialComp.Colour);

                    CurrentRHC->DrawElements(submesh.IndexCount, submesh.IndexStart);
                }

                meshComp.VertexArray.UnBind();
                shader->UnBind();
            }
        }
    }

    void RenderPrimitive(level::PRIMITIVE_TYPES primitve, math::Vec3 position, math::Vec3 rotation, math::Vec3 scale, math::Vec3 colour)
    {
        static graphics::VertexArray cubeVA;
        static graphics::VertexArray sphereVA;

        static graphics::BufferLayout layout = 
        {
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aPos"},
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aNormal"}
        };

        static bool initialised = false;
        
        if (!initialised)
        {
            cubeVA.Create();
            PrimitiveCubeVB->SetLayout(layout);
            cubeVA.AddVertexBuffer(PrimitiveCubeVB);
            cubeVA.SetIndexBuffer(PrimitiveCubeIB);

            sphereVA.Create();
            PrimitiveSphereVB->SetLayout(layout);
            sphereVA.AddVertexBuffer(PrimitiveSphereVB);
            sphereVA.SetIndexBuffer(PrimitiveSphereIB);

            initialised = true;
        }

        math::Matrix4 rotationMatrix = math::ToMatrix4(math::Quat(rotation));
        math::Matrix4 model = math::Translate(math::Matrix4(1.0f), position) * rotationMatrix;
        model = math::Scale(model, scale);

        auto* shader = CurrentShaderManager->Get(PrimitiveShader);

        shader->Bind();

        int windowWidth, windowHeight;
        SDL_GetWindowSize(CurrentRHC->GetWindow(), &windowWidth, &windowHeight);

        shader->SetVec3("uColour", colour);
        shader->SetMat4("uProjection", Camera.GetProjection(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
        shader->SetMat4("uView", Camera.GetViewMatrix());
        shader->SetMat4("uModel", model);

        switch (primitve)
        {
            case level::PRIMITIVE_TYPES::CUBE:
                cubeVA.Bind();
                CurrentRHC->DrawElements(cubeVA.GetIndexBuffer()->GetCount(), 0);
                break;
            case level::PRIMITIVE_TYPES::SPHERE:
                sphereVA.Bind();
                CurrentRHC->DrawElements(sphereVA.GetIndexBuffer()->GetCount(), 0);
                break;
            default:
                break;
        }

        cubeVA.UnBind();
        sphereVA.UnBind();

        shader->UnBind();
    }
}