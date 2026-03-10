#ifndef _HENLEVEL_PRIMITIVES_H_
#define _HENLEVEL_PRIMITIVES_H_

#include "core/henMath.h"

#include <cstdint>

namespace hen::level
{
    class Entity;
}

namespace hen::level::primitives
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
}

#endif // !_HENLEVEL_PRIMITIVES_H_