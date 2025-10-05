#ifndef _HENSCENE_H_
#define _HENSCENE_H_

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

namespace hen::scene
{
    namespace actors
    {
        class Camera
        {
        public:

            Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

            glm::mat4 GetViewMatrix();

            void SetDirty();

        public:

            glm::vec3 Position;
            glm::vec3 Front;
            glm::vec3 Up;
            glm::vec3 Right;

            float Yaw = -90.0f;
            float Pitch = 0.0f;
            float Speed = 2.0f;
            float FOV = 90.0f;
        };
    }
    
    extern glm::vec3 Up;
}


#endif // !_HENSCENE_H_