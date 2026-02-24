#ifndef _HENPHYSICS_H_
#define _HENPHYSICS_H_

#include "level/henLevel.h"
#include "core/henMath.h"

namespace hen::physics
{
    void Initialise();
    void Shutdown();

    void Update(float deltaTime);

    void AddImpulse(level::RigidBodyComponent& rbComponent, math::Vec3 impulse);

    void AddForce(level::RigidBodyComponent& rbComponent, math::Vec3 force);

    void AddTorque(level::RigidBodyComponent& rbComponent, math::Vec3 torque);

    extern bool Initialised;
}

#endif //_HENPHYSICS_H_