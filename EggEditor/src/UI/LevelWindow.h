#ifndef _LEVELWINDOW_H_
#define _LEVELWINDOW_H_

#include "HENEngine.h"

#include "ComponentWindow.h"

class LevelWindow
{
public:
    void Initialise(ComponentWindow* compWindow);
    
private:
    ComponentWindow* m_CompWindow;
    
    hen::renderer::TextureHandle m_EntityIcon;

};

#endif // !_LEVELWINDOW_H_