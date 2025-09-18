#include "level/henLevel.h"


namespace hen::level
{
    Level::Level()
    {

    }

    Level::~Level()
    {

    }

    void Level::Update(float deltaTime)
    {

    }

    Entity Level::CreateEntity(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};

        if (name.empty())
        {
            entity.AddComponent<NameComponent>("unknown");
        }
        else
        {
            entity.AddComponent<NameComponent>(name);
        }

        return entity;
    }

    Entity::Entity(const Entity& other)
    {

    }

    Entity::Entity(entt::entity handle, Level* level)
        : m_Handle(handle), m_Level(level)
    {
        
    }

}
