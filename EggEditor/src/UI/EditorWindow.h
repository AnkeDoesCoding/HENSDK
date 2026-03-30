#ifndef _EDITORWINDOW_H_
#define _EDITORWINDOW_H_

#include "HENEngine.h"

class EditorWindow
{
public:
    void Intialise();

private:
    hen::renderer::TextureHandle m_SaveIcon;
    hen::renderer::TextureHandle m_OpenIcon;
    hen::renderer::TextureHandle m_UndoIcon;
    hen::renderer::TextureHandle m_RedoIcon;

    bool m_OpenControlWindow = false;
};

#endif // !_EDITORWINDOW_H_