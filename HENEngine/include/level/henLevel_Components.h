#ifndef _HENLEVEL_COMPONENTS_H_
#define _HENLEVEL_COMPONENTS_H_

#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include <vendor/glm/gtx/matrix_decompose.hpp>
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>

#include "graphics/henGraphics.h"
#include "renderer/henRenderer_ResourceManagers.h"

#include <string>

namespace hen::level
{
    
    struct NameComponent
    {
        std::string Name = "unknown";

        NameComponent() = default;

        NameComponent(const NameComponent& other) = default;

        NameComponent(const std::string& name)
            : Name(name)
        {

        }
    };

    struct TransformComponent
    {
        glm::mat4 Transform = glm::mat4(1.0f);

        TransformComponent() = default;

        TransformComponent(const TransformComponent& other) = default;

        TransformComponent(const glm::mat4& transform)
            : Transform(transform)
        {

        }

        TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
        {
            Transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
        }

        glm::vec3 GetRotation()
        {
            glm::vec3 scale, translation, skew;
            glm::vec4 perspective;
            glm::quat rotation;

            glm::decompose(Transform, scale, rotation, translation, skew, perspective);

            return glm::eulerAngles(rotation);
        }

        glm::vec3 GetPosition()
        {
            glm::vec3 scale, translation, skew;
            glm::vec4 perspective;
            glm::quat rotation;

            glm::decompose(Transform, scale, rotation, translation, skew, perspective);

            return translation;
        }

        glm::vec3 GetScale()
        {
            glm::vec3 scale, translation, skew;
            glm::vec4 perspective;
            glm::quat rotation;

            glm::decompose(Transform, scale, rotation, translation, skew, perspective);

            return scale;
        }

        void SetPosition(const glm::vec3& position)
        {
            auto rotation = GetRotation();
            auto scale = GetScale();

            Transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
        }

        void SetRotation(const glm::vec3& rotation)
        {
            auto position = GetPosition();
            auto scale = GetScale();

            Transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
        }

        void SetScale(const glm::vec3& scale)
        {
            auto position = GetPosition();
            auto rotation = GetRotation();

            Transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
        }

        operator glm::mat4& ()
        {
            return Transform;
        }

        operator const glm::mat4& () const
        {
            return Transform;
        }

    };

    struct MeshComponent
    {
        std::vector<glm::vec3> Verticies;
        std::vector<glm::vec3> Normals;
        std::vector<uint32_t> Indicies;

        std::shared_ptr<graphics::VertexArray> VertexArray;
        std::shared_ptr<graphics::VertexBuffer> VertexBuffer;
        std::shared_ptr<graphics::IndexBuffer> IndexBuffer;

        graphics::BufferLayout BufferLayout
        {
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aPos"},
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aNormal"}
        };

        unsigned int ID;

        MeshComponent() = default;

        MeshComponent(const MeshComponent& other) = default;

        void CreateRenderData()
        {
            HEN_ASSERT(Verticies.size() == Normals.size(), "Positions and Normals size mismatch!");

            std::vector<float> interleavedBuffer;
            interleavedBuffer.reserve(Verticies.size() * 6);

            for (size_t i = 0; i < Verticies.size(); i++)
            {
                interleavedBuffer.push_back(Verticies[i].x);
                interleavedBuffer.push_back(Verticies[i].y);
                interleavedBuffer.push_back(Verticies[i].z);

                interleavedBuffer.push_back(Normals[i].x);
                interleavedBuffer.push_back(Normals[i].y);
                interleavedBuffer.push_back(Normals[i].z);
            }
            
            VertexBuffer = graphics::VertexBuffer::Create(interleavedBuffer.size() * sizeof(float), interleavedBuffer.data());
            VertexBuffer->SetLayout(BufferLayout);

            IndexBuffer = graphics::IndexBuffer::Create(Indicies.size(), Indicies.data());

            VertexArray = graphics::VertexArray::Create();
            VertexArray->AddVertexBuffer(VertexBuffer);
            VertexArray->SetIndexBuffer(IndexBuffer);

        }

        void DeleteRenderData()
        {
            if (VertexArray)
            {
                VertexArray->UnBind();
                VertexArray.reset();
            }
        
            if (VertexBuffer)
            {
                VertexBuffer.reset();
            }
        
            if (IndexBuffer)
            {
                IndexBuffer.reset();
            }
        }

    };

    struct MaterialComponent
    {
        graphics::Texture2D DiffuseTexture;
        graphics::Texture2D SpecularTexture;

        renderer::ShaderHandle Shader;

        MaterialComponent()
        {
            if (DiffuseTexture.ID != 0 && SpecularTexture.ID != 0)
            {
                Shader = renderer::GetShaderManager()->Load(ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/LitShaderFS.glsl");
            }
            else
            {
                Shader = renderer::GetShaderManager()->Load(ENGINE_RESOURCE_PATH "shaders/GLSL/PrimitiveShaderVS.glsl",ENGINE_RESOURCE_PATH "shaders/GLSL/PrimitiveShaderFS.glsl");
            }
        }
    };

    struct CameraComponent
    {
        float FOV = 90.0f;

        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 90.0f, 0.0f);
        glm::vec3 Front;
        glm::vec3 Right;
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        CameraComponent() = default;

        CameraComponent(const CameraComponent& other) = default;

        CameraComponent(const float& fov, const glm::vec3& position, const glm::vec3& rotation)
            : FOV(fov), Position(position), Rotation(rotation)
        {

        } 

        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        void SetDirty(const glm::vec3& levelUp)
        {
            glm::vec3 front;

            Rotation.x = glm::clamp(Rotation.x, -89.99f, 89.99f); // you can never truly look 90 up or down, hopefully this doesnt fuck up future calculations

            front.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
            front.y = sin(glm::radians(Rotation.x));
            front.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
            Front = glm::normalize(front);

            Right = glm::normalize(glm::cross(Front, levelUp));
            Up = glm::normalize(glm::cross(Right, Front));
        }

    };  
}

#endif // !_HENLEVEL_COMPONENTS_H_