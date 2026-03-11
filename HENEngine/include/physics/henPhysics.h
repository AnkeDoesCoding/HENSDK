#ifndef _HENPHYSICS_H_
#define _HENPHYSICS_H_

#include "level/henLevel.h"
#include "core/henMath.h"

namespace hen::physics
{
    void Initialise();
    void Shutdown();

    void Update(float deltaTime);

    math::Vec3 GetLinearVelocity(level::RigidBodyComponent& rbComponent);    
    math::Vec3 GetAngularVelocity(level::RigidBodyComponent& rbComponent);

    void AddImpulse(level::RigidBodyComponent& rbComponent, const math::Vec3& impulse);
    void AddImpulseAt(level::RigidBodyComponent& rbComponent, const math::Vec3& impulse, const math::Vec3& position);

    void AddForce(level::RigidBodyComponent& rbComponent, const math::Vec3& force);
    void AddForceAt(level::RigidBodyComponent& rbComponent, const math::Vec3& force, const math::Vec3& position);

    void AddTorque(level::RigidBodyComponent& rbComponent, const math::Vec3& torque);
    
    level::RayResult CastRay(const level::Ray& ray);

    extern bool Initialised;
}

#endif //_HENPHYSICS_H_