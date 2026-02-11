#include "level/henLevel.h"


namespace hen::level
{
    Level* ActiveLevel = nullptr;

    Level::Level()
    {
        ActiveLevel = this;
    }

    Level::~Level()
    {
        if (ActiveLevel == this)
        {
            ActiveLevel = nullptr;
        }
    }

    void Level::Update(float deltaTime)
    {

    }

    entt::registry* Level::GetRegistry()
    {
        return &m_Registry;
    }

    Entity Level::CreateEntity(const std::string& name)
    {
        if (!GetActiveLevel())
        {
            HEN_ERROR("[hen::level] Attempted to create entity '" + name + "' but no level found");

            return Entity{};
        }

        Entity entity = {m_Registry.create(), this};

        entity.AddComponent<NameComponent>(name.empty() ? "unknown" : name);

        return entity;
   }

    Entity::Entity(const Entity& other)
        : m_Handle(other.m_Handle), m_Level(other.m_Level)
    {

    }

    Entity::Entity(entt::entity handle, Level* level)
        : m_Handle(handle), m_Level(level)
    {
        
    }

    Level* GetActiveLevel() 
    { 
        return ActiveLevel; 
    }



}
