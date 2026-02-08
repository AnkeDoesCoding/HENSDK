#ifndef _COMPONENTWINDOW_H_
#define _COMPONENTWINDOW_H_

#include "HENEngine.h"

class ComponentWindow
{
public:
    void Initialise();
    
public:
    hen::level::Entity SelectedEntity;

private:

    hen::level::Entity m_LastSelectedEntity;

    hen::math::Vec3 m_DegreesRotation;
    bool m_RotationInitialised;
};

#endif // !_COMPONENTWINDOW_H_