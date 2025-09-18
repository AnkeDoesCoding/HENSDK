#ifndef _HENLEVEL_H_
#define _HENLEVEL_H_

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/entt/include/entt.hpp"

#include "tools/henConsole.h"


namespace hen::level
{

    class Level
    {
        friend class Entity;
    public:
        Level();
        ~Level();

        void Update(float deltaTime);
    
        Entity CreateEntity(const std::string& name = std::string());

    public:

        glm::vec3 Up;

    private:
        
        entt::registry m_Registry;
        
    };

    class Entity
    {
    public:
        Entity(const Entity& other);
        Entity(entt::entity handle, Level* level);

        template<typename T>
        bool HasComponent() const
        {
            return m_Level->m_Registry.valid(m_Handle) && m_Level->m_Registry.all_of<T>(m_Handle);
        }

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            HEN_ASSERT(!HasComponent<T>(), "Entity already has this component");

            return m_Level->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            HEN_ASSERT(!HasComponent<T>(), "Entity doesn't have this component");

            return m_Level->m_Registry.get<T>(m_Handle);
        }

        template<typename T>
        void RemoveComponent()
        {
            HEN_ASSERT(!HasComponent<T>(), "Entity doesn't have this component");
            m_Level->m_Registry.remove<T>(m_Handle);
        }

        operator bool() const
        {
            return m_Handle != entt::null;
        }

    private:
        entt::entity m_Handle {0};
        Level* m_Level = nullptr;

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
        glm::mat4 Transform;

        TransformComponent() = default;
        TransformComponent(const TransformComponent& other) = default;
        TransformComponent(const glm::mat4& transform)
            : Transform(transform)
        {

        }

        glm::vec3 GetRotation()
        {
            // return
        }

        glm::vec3 GetPosition()
        {
            // return
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
}

#endif // !_HENLEVEL_H_