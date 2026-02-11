#ifndef _HENPHYSICS_H_
#define _HENPHYSICS_H_

namespace hen::physics
{
    void Initialise();
    void Shutdown();

    void Update(float deltaTime);

    extern bool Initialised;
}

#endif //_HENPHYSICS_H_