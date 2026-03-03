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
};

#endif // !_COMPONENTWINDOW_H_