#ifndef _HENLEVEL_H_
#define _HENLEVEL_H_

#include "vendor/entt/include/entt.hpp"

#include "tools/henConsole.h"
#include "level/henLevel_Components.h"

#define UNDERLYING_VIEW decltype(std::declval<entt::registry>().view<Components...>())

namespace hen::level
{
    class Entity;

    class Level
    {
        friend class Entity;
    public:
        Level();
        ~Level();

        void Update(float deltaTime);

        entt::registry* GetRegistry();
    
        Entity CreateEntity(const std::string& name = std::string());

        template<typename... Components>
        auto GetView() 
        {
            return View<Components...>(m_Registry, this);
        }

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

        template<typename Component>
        bool HasComponent() const
        {
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
            HEN_ASSERT(HasComponent<Component>(), "Entity doesn't have this component");

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
            return m_Handle != entt::null;
        }

        operator entt::entity() const 
        { 
            return m_Handle; 
        }

    private:
        entt::entity m_Handle { entt::null };
        Level* m_Level = nullptr;

    };

    template<typename... Components>
    class View
    {
    public:

        explicit View(entt::registry& registry, Level* level)
            : m_View(registry.view<Components...>()), m_Level(level) 
        {

        }

        class Iterator
        {
        public:
            Iterator(typename UNDERLYING_VIEW::iterator it, Level* level)
                : m_It(it), m_Level(level) 
            {
                
            }

            Entity operator*() const 
            { 
                return Entity(*m_It, m_Level); 
            }

            Iterator& operator++() 
            { 
                ++m_It; return *this; 
            }

            bool operator!=(const Iterator& other) const 
            { 
                return m_It != other.m_It; 
            }

        private:
            typename UNDERLYING_VIEW::iterator m_It;
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

        template<typename... T>
        auto Get(Entity entity)
        {
            return m_View.template get<T...>((entt::entity)entity);
        }

        bool Contains(Entity entity) const
        {
            return m_View.contains((entt::entity)entity);
        }

    private:
        UNDERLYING_VIEW m_View;
        Level* m_Level = nullptr;
    };

   Level* GetActiveLevel();
    
}

#endif // !_HENLEVEL_H_