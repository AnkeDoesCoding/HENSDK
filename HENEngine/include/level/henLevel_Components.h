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
    enum class LIGHT_TYPES
    {
        POINT,
        SPOT,
        DIRECTIONAL
    };
    
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
        mutable glm::mat4 Transform;
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

        mutable bool Dirty = true;

        TransformComponent() = default;

        TransformComponent(const TransformComponent& other) = default;

        TransformComponent(const glm::mat4& transform)
            : Transform(transform)
        {

        }

        TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
            : Position(position), Rotation(rotation), Scale(scale)
        {
            
        }

        const glm::vec3& GetRotation() const
        {
            return Rotation;
        }

        const glm::vec3& GetPosition()
        {
            return Position;
        }

        const glm::vec3& GetScale() const
        {
            return Scale;
        }

        const glm::mat4& GetMatrix() const
        {
            if (Dirty)
            {
                Transform = glm::translate(glm::mat4(1.0f), Position) * glm::toMat4(glm::quat(Rotation)) * glm::scale(glm::mat4(1.0f), Scale);

                Dirty = false;
            }

            return Transform;
        }

        glm::vec3 GetForwardVector() const
        {
            float pitch = Rotation.x;
            float yaw   = Rotation.y;

            glm::vec3 forward;
            forward.x = cos(yaw) * cos(pitch);
            forward.y = sin(pitch);
            forward.z = sin(yaw) * cos(pitch);

            return glm::normalize(forward);
        }

        glm::vec3 GetUpVector() const
        {
            glm::vec3 levelUp = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 forward = GetForwardVector();
            glm::vec3 right   = glm::normalize(glm::cross(forward, levelUp));
            return glm::normalize(glm::cross(right, forward));
        }

        glm::vec3 GetRightVector() const
        {
            glm::vec3 levelUp = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 forward = GetForwardVector();
            return glm::normalize(glm::cross(forward, levelUp));
        }

        void SetPosition(const glm::vec3& position)
        {
            Position = position;
            Dirty = true;
        }

        void SetRotation(const glm::vec3& rotation)
        {
            Rotation = rotation;
            Dirty = true;
        }

        void SetScale(const glm::vec3& scale)
        {
            Scale = scale;
            Dirty = true;
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

    struct MaterialComponent
    {
        renderer::TextureHandle DiffuseTexture;
        renderer::TextureHandle SpecularTexture;

        renderer::ShaderHandle Shader;

        MaterialComponent() = default;

        MaterialComponent(const MaterialComponent& other) = default;
    };

    struct MeshComponent
    {
        std::vector<glm::vec3> Vertices;
        std::vector<glm::vec3> Normals;
        std::vector<uint32_t> Indices;
        std::vector<glm::vec2> TextureCoordinates;

        graphics::VertexArray VertexArray;
        std::shared_ptr<graphics::VertexBuffer> VertexBuffer;
        std::shared_ptr<graphics::IndexBuffer> IndexBuffer;

        graphics::BufferLayout BufferLayout
        {
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aPos"},
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aNormal"},
            {graphics::SHADER_PRIMITIVES::FLOAT2, "aTexCoord"}

        };

        struct SubMesh
        {
            uint32_t IndexStart = UINT32_MAX;
            uint32_t IndexCount = UINT32_MAX;

            MaterialComponent Material;
        };

        std::vector<SubMesh> SubMeshes;

        MeshComponent() = default;

        MeshComponent(const MeshComponent& other) = default;

        void CreateRenderData()
        {
            HEN_ASSERT(Vertices.size() == Normals.size(), "Positions and Normals size mismatch");

            std::vector<float> interleavedBuffer;
            interleavedBuffer.reserve(Vertices.size() * 8);

            for (size_t i = 0; i < Vertices.size(); i++)
            {
                interleavedBuffer.push_back(Vertices[i].x);
                interleavedBuffer.push_back(Vertices[i].y);
                interleavedBuffer.push_back(Vertices[i].z);

                Normals[i] = glm::normalize(Normals[i]);

                interleavedBuffer.push_back(Normals[i].x);
                interleavedBuffer.push_back(Normals[i].y);
                interleavedBuffer.push_back(Normals[i].z);

                if (i < TextureCoordinates.size())
                {
                    interleavedBuffer.push_back(TextureCoordinates[i].x);
                    interleavedBuffer.push_back(TextureCoordinates[i].y);
                }
                else
                {
                    interleavedBuffer.push_back(0.0f);
                    interleavedBuffer.push_back(0.0f);
                }

            }
            
            VertexBuffer = graphics::VertexBuffer::Create(interleavedBuffer.size() * sizeof(float), interleavedBuffer.data());
            VertexBuffer->SetLayout(BufferLayout);

            IndexBuffer = graphics::IndexBuffer::Create(Indices.size(), Indices.data());

            VertexArray.Create();
            VertexArray.AddVertexBuffer(VertexBuffer);
            VertexArray.SetIndexBuffer(IndexBuffer);

        }

        void DeleteRenderData()
        {
            if (VertexArray.IsBackendValid())
            {
                VertexArray.UnBind();
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

    struct LightComponent
    {
        LIGHT_TYPES Type;

        float Range;
        float Intensity;
        float InnerCutOff;
        float OuterCutOff;

        glm::vec3 Colour = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ambient = glm::vec3(0.0f, 0.0f, 0.0f);

        LightComponent() = default;

        LightComponent(const LightComponent& other) = default;

        LightComponent(LIGHT_TYPES type, float intensity)
            : Type(type), Intensity(intensity)
        {

        }
    };

    struct CameraComponent
    {
        float FOV = 90.0f;
        float NearPlane = 3.0f;
        float FarPlane = 1500.0f;

        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
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
        
        glm::mat4 GetProjection(float x, float y)
        {
            return glm::perspective(glm::radians(FOV), x / y, NearPlane, FarPlane);
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