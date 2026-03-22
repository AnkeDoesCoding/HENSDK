#include "level/henLevel.h"

#include "renderer/henRenderer.h"

namespace hen::level
{
    namespace cube
    {
        float Vertices[] =
        {
            // vertices           // normals
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,-1.0f,
             0.5f, -0.5f, -0.5f,   0.0f, 0.0f,-1.0f,
             0.5f,  0.5f, -0.5f,   0.0f, 0.0f,-1.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,-1.0f,
            -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f,-1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f,-1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   0.0f,-1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   0.0f,-1.0f, 0.0f
        };

        uint32_t Indices[] = 
        {
            0, 1, 2,  2, 3, 0,
            4, 5, 6,  6, 7, 4,
            8, 9,10,  10,11,8,
            12,13,14, 14,15,12,
            16,17,18, 18,19,16,
            20,21,22, 22,23,20
        };
    }

    namespace sphere
    {
        float Vertices[] =
        {
            // vertices                // normals
             0.0f,    1.0f,    0.0f,    0.0f,    1.0f,    0.0f,
             0.0f,    0.707f,  0.707f,  0.0f,    0.707f,  0.707f,
             0.5f,    0.707f,  0.5f,    0.5f,    0.707f,  0.5f,
             0.707f,  0.707f,  0.0f,    0.707f,  0.707f,  0.0f,
             0.5f,    0.707f, -0.5f,    0.5f,    0.707f, -0.5f,
             0.0f,    0.707f, -0.707f,  0.0f,    0.707f, -0.707f,
            -0.5f,    0.707f, -0.5f,   -0.5f,    0.707f, -0.5f,
            -0.707f,  0.707f,  0.0f,   -0.707f,  0.707f,  0.0f,
            -0.5f,    0.707f,  0.5f,   -0.5f,    0.707f,  0.5f,
             0.0f,    0.0f,    1.0f,    0.0f,    0.0f,    1.0f,
             0.707f,  0.0f,    0.707f,  0.707f,  0.0f,    0.707f,
             1.0f,    0.0f,    0.0f,    1.0f,    0.0f,    0.0f,
             0.707f,  0.0f,   -0.707f,  0.707f,  0.0f,   -0.707f,
             0.0f,    0.0f,   -1.0f,    0.0f,    0.0f,   -1.0f,
            -0.707f,  0.0f,   -0.707f, -0.707f,  0.0f,   -0.707f,
            -1.0f,    0.0f,    0.0f,   -1.0f,    0.0f,    0.0f,
            -0.707f,  0.0f,    0.707f, -0.707f,  0.0f,    0.707f,
             0.0f,   -0.707f,  0.707f,  0.0f,   -0.707f,  0.707f,
             0.5f,   -0.707f,  0.5f,    0.5f,   -0.707f,  0.5f,
             0.707f, -0.707f,  0.0f,    0.707f, -0.707f,  0.0f,
             0.5f,   -0.707f, -0.5f,    0.5f,   -0.707f, -0.5f,
             0.0f,   -0.707f, -0.707f,  0.0f,   -0.707f, -0.707f,
            -0.5f,   -0.707f, -0.5f,   -0.5f,   -0.707f, -0.5f,
            -0.707f, -0.707f,  0.0f,   -0.707f, -0.707f,  0.0f,
            -0.5f,   -0.707f,  0.5f,   -0.5f,   -0.707f,  0.5f,
             0.0f,   -1.0f,    0.0f,    0.0f,   -1.0f,    0.0f
        };

        uint32_t Indices[] = 
        {
            0,1,2, 0,2,3, 0,3,4, 0,4,5,
            0,5,6, 0,6,7, 0,7,8, 0,8,1,
            1,9,2, 2,9,10, 2,10,3, 3,10,11,
            3,11,4, 4,11,12, 4,12,5, 5,12,13,
            5,13,6, 6,13,14, 6,14,7, 7,14,15,
            7,15,8, 8,15,16, 8,16,1, 1,16,9,
            9,17,10, 10,17,18, 10,18,11, 11,18,19,
            11,19,12, 12,19,20, 12,20,13, 13,20,21,
            13,21,14, 14,21,22, 14,22,15, 15,22,23,
            15,23,16, 16,23,24, 16,24,9, 9,24,17,
            25,18,17, 25,19,18, 25,20,19, 25,21,20,
            25,22,21, 25,23,22, 25,24,23, 25,17,24
        };

    }

    Ray::Ray(const math::Vec3& origin, const math::Vec3& direction, const float min, const float max)
        :Origin(origin), Direction(direction), Minimum(min), Maximum(max)
    {

    }

    Ray::Ray(const math::Vec2& mousePos, const float min, const float max)
    {
        int windowHeight;
        SDL_GetWindowSize(renderer::GetRHC()->GetWindow(), NULL, &windowHeight);

        graphics::Viewport viewport = renderer::GetRHC()->GetViewport();

        float localX = mousePos.x - viewport.Position.x;
        float localY = mousePos.y - (windowHeight - viewport.Position.y - viewport.Size.y);;

        float ndcX = (2.0f * localX) / viewport.Size.x - 1.0f;
        float ndcY = 1.0f - (2.0f * localY) / viewport.Size.y;

        math::Vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);

        math::Vec4 rayEye = math::Inverse(renderer::Camera.GetProjection(static_cast<float>(viewport.Size.x), static_cast<float>(viewport.Size.y))) * rayClip;
        rayEye = math::Vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        math::Vec4 rayWorld = math::Inverse(renderer::Camera.GetViewMatrix()) * rayEye;
        math::Vec3 rayDir = math::Normalise(math::Vec3(rayWorld.x, rayWorld.y, rayWorld.z));

        Origin = renderer::Camera.Position;
        Direction = rayDir;
        Minimum = min;
        Maximum = max;
    }

    Ray::Ray(const math::Vec3& start, const math::Vec3& end)
    {
        Origin = start;
        Direction = math::Normalise(end - start);
        Maximum = math::Length(end - start);
    }

    static Level* ActiveLevel = nullptr;

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

    Entity Level::CreateEntity(const std::string& name)
    {
        if (!GetActiveLevel())
        {
            HEN_WARN("[hen::level] Attempt to create entity: " + name + " failed due to no level being loaded");
            return Entity{};
        }

        Entity entity = {m_Registry.create(), this};

        entity.AddComponent<NameComponent>(name.empty() ? "unknown" : name);

        return entity;
    }

    void Level::RemoveEntity(const Entity& entity)
    {
        m_Registry.destroy(entity);
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
