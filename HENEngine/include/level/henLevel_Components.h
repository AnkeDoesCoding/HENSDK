#ifndef _HENLEVEL_COMPONENTS_H_
#define _HENLEVEL_COMPONENTS_H_

#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include <vendor/glm/gtx/matrix_decompose.hpp>
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>

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
        std::vector<float> Verticies;
        std::vector<uint32_t> Indicies;

        MeshComponent() = default;

        MeshComponent(const MeshComponent& other) = default;

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