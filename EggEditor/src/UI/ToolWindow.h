#ifndef _TOOLWINDOW_H_
#define _TOOLWINDOW_H_

#include "HENEngine.h"

#include "ComponentWindow.h"

class ToolWindow
{
public:
    void Initialise(ComponentWindow* compWindow);

private:
    hen::renderer::TextureHandle m_TranslateIcon;
    hen::renderer::TextureHandle m_RotateIcon;
    hen::renderer::TextureHandle m_ScaleIcon;
    hen::renderer::TextureHandle m_AddIcon;
    hen::renderer::TextureHandle m_RemoveIcon;

    ComponentWindow* m_ComponentWindow;    

};

#endif // !_TOOLWINDOW_H_