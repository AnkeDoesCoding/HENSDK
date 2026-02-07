#ifndef _HENPHYSICS_H_
#define _HENPHYSICS_H_

namespace hen::physics
{
    void Initialise();
    void Shutdown();

    void Run(float hz);
}

#endif //_HENPHYSICS_H_