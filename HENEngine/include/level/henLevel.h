#ifndef _HENLEVEL_H_
#define _HENLEVEL_H_

#include "core/henMath.h"
#include "level/henLevel_Components.h"
#include "tools/henConsole.h"

#include <entt/include/entt.hpp>

#include <cstdint>

namespace hen::level
{
    namespace cube
    {
        extern float Vertices[144];
        extern uint32_t Indices[36];
    }

    namespace sphere
    {
        extern float Vertices[156];
        extern uint32_t Indices[144];
    }

    class Entity;

    template<typename... Components>
    class View;

    enum class PRIMITIVE_TYPES
    {
        CUBE,
        SPHERE
    };

    struct Ray
    {
        math::Vec3 Origin = math::Vec3(0.0f);
        math::Vec3 Direction = math::Vec3(0.0f);
        float Minimum = 0.0f;
        float Maximum = 1000.0f;

        Ray(const math::Vec3& origin, const math::Vec3& direction, const float min, const float max);
        Ray(const math::Vec2& mousePos, const float min, const float max);
        Ray(const math::Vec3& start, const math::Vec3& end);
    };

    struct RayResult
    {
        bool Hit = false;
        Entity* HitEntity = nullptr;
        math::Vec3 HitPosition = math::Vec3(0.0f);
        math::Vec3 HitNormal = math::Vec3(0.0f);
    };

    class Level
    {
        friend class Entity;
    public:
        Level();
        ~Level();
    
        Entity CreateEntity(const std::string& name = std::string());
        void RemoveEntity(const Entity& entity);

        template<typename... Components>
        auto GetView() 
        {
            return View<Components...>(m_Registry, this);
        }

    public:
        math::Vec3 Up = math::Vec3(0.0f, 1.0f, 0.0f);
        math::Vec3 Gravity = math::Vec3(0.0f, -20.0f, 0.0f); // -9.81 felt too floaty even though it was physically correct
        
        std::shared_ptr<void> PhysicsLevel;  // I dont really like this shared void ptr approach

    private:
        entt::registry m_Registry;
        
    };

    class Entity
    {
    public:
        Entity() = default;
        Entity(const Entity& other);
        Entity(entt::entity handle, Level* level);

        bool IsValid() const
        {
            return m_Handle != entt::null && m_Level != nullptr && m_Level->m_Registry.valid(m_Handle);
        }

        template<typename Component>
        bool HasComponent() const
        {
            HEN_ASSERT(IsValid(), "Entity isn't valid");
            return m_Level->m_Registry.valid(m_Handle) && m_Level->m_Registry.all_of<Component>(m_Handle);
        }      

        template<typename Component, typename... Args>
        Component& AddComponent(Args&&... args)
        {
            HEN_ASSERT(!HasComponent<Component>(), "Entity already has this component");

            return m_Level->m_Registry.emplace<Component>(m_Handle, std::forward<Args>(args)...);
        }

        template<typename Component>
        Component& GetComponent()
        {
            return m_Level->m_Registry.get<Component>(m_Handle);
        }

        template<typename Component>
        void RemoveComponent()
        {
            HEN_ASSERT(HasComponent<Component>(), "Entity doesn't have this component");
            
            m_Level->m_Registry.remove<Component>(m_Handle);
        }

        operator bool() const
        {
            return IsValid();
        }

        operator entt::entity() const 
        { 
            return m_Handle; 
        }

        bool operator==(const Entity& other) const
        {
            return m_Handle == other.m_Handle && m_Level == other.m_Level;
        }
    
        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

    private:
        entt::entity m_Handle { entt::null };
        Level* m_Level = nullptr;

    };

    template<typename... Components>
    class View
    {
        using UnderlyingView = decltype(std::declval<entt::registry>().view<Components...>());
    public:

        explicit View(entt::registry& registry, Level* level)
            : m_View(registry.view<Components...>()), m_Level(level) 
        {

        }

        class Iterator
        {
        public:
            Iterator(typename UnderlyingView::iterator it, Level* level)
                : m_It(it), m_Level(level) 
            {
                
            }

            Entity operator*() const 
            { 
                return Entity(*m_It, m_Level); 
            }

            Iterator& operator++() 
            { 
                m_It++; 
                return *this; 
            }

            bool operator!=(const Iterator& other) const 
            { 
                return m_It != other.m_It; 
            }

        private:
            typename UnderlyingView::iterator m_It;
            Level* m_Level;

        };

        Iterator begin() 
        { 
            return Iterator(m_View.begin(), m_Level); 
        }

        Iterator end()   
        { 
            return Iterator(m_View.end(),   m_Level); 
        }

        Entity operator[](std::size_t index) const
        {
            auto it = m_View.begin();
            std::advance(it, index);
            return Entity(*it, m_Level);
        }

        template<typename... Component>
        auto Get(Entity entity)
        {
            return m_View.template get<Component...>(static_cast<entt::entity>(entity));
        }

        bool Contains(Entity entity) const
        {
            return m_View.contains(static_cast<entt::entity>(entity));
        }

        // Only works when view contains one type of component  
        std::size_t Size() const
        {
            return m_View.size(); 
        }

    private:
        UnderlyingView m_View;
        Level* m_Level = nullptr;
        
    };

   Level* GetActiveLevel();
    
}

#endif // !_HENLEVEL_H_