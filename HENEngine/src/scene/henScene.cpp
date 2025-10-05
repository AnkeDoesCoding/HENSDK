#include "scene/henScene.h"

namespace hen::scene
{
    glm::vec3 Up(0.0f, 1.0f, 0.0f);

    namespace actors
    {
        Camera::Camera(glm::vec3 pos, float yaw, float pitch)
            : Position(pos), Yaw(yaw), Pitch(pitch), Up(scene::Up)
        {
            SetDirty();
        }

        glm::mat4 Camera::GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        void Camera::SetDirty()
        {
            glm::vec3 front;

            Pitch = glm::clamp(Pitch, -89.99f, 89.99f); // you can never truly look 90 up or down, hopefully this doesnt fuck up future calculations

            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);

            Right = glm::normalize(glm::cross(Front, scene::Up));
            Up    = glm::normalize(glm::cross(Right, Front));
        }
    }
    

}