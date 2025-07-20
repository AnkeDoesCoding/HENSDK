#include "scene/henSceneActors.h"

namespace hen::scene::actors
{
    Camera::Camera(glm::vec3 pos, float yaw, float pitch)
    {
        Position = pos;
        Yaw = yaw;
        Pitch = pitch;
        Up = glm::vec3(0.0f, 1.0f, 0.0f);

        SetDirty();
    }

    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void Camera::SetDirty()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }

}