#ifndef _HENPHYSICS_H_
#define _HENPHYSICS_H_

#include "level/henLevel.h"
#include "core/henMath.h"

namespace hen::physics
{
    void Initialise();
    void Shutdown();

    void Update(float deltaTime);

    void AddImpulse(level::RigidBodyComponent& rbComponent, const math::Vec3& impulse);
    void AddImpulseAt(level::RigidBodyComponent& rbComponent, const math::Vec3& impulse, const math::Vec3& position);

    void AddForce(level::RigidBodyComponent& rbComponent, const math::Vec3& force);
    void AddForceAt(level::RigidBodyComponent& rbComponent, const math::Vec3& force, const math::Vec3& position);

    void AddTorque(level::RigidBodyComponent& rbComponent, const math::Vec3& torque);
    
    level::primitives::RayResult CastRay(const level::primitives::Ray& ray);

    extern bool Initialised;
}

#endif //_HENPHYSICS_H_