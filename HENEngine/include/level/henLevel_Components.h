#ifndef _HENLEVEL_COMPONENTS_H_
#define _HENLEVEL_COMPONENTS_H_

#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include <vendor/glm/gtx/matrix_decompose.hpp>
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>

#include "graphics/henGraphics.h"

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
        glm::mat4 Transform;

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

    struct CameraComponent
    {
        float FOV = 90.0f;

        CameraComponent() = default;

        CameraComponent(const CameraComponent& other) = default;

        CameraComponent(const float& fov)
            : FOV(fov)
        {

        }

    };
}

#endif // !_HENLEVEL_COMPONENTS_H_