#ifndef _EGGEDITOR_H_
#define _EGGEDITOR_H_

#include "HENEngine.h"

#include "UI/LevelWindow.h"
#include "UI/ComponentWindow.h"
#include "UI/ToolWindow.h"
#include "UI/EditorWindow.h"

class Editor : public hen::Application
{
public:
    void Initialise(SDL_Window* window) override;
    void Shutdown() override;

    void FixedUpdate() override;
    void Update(float deltaTime) override;

private:
    ComponentWindow m_ComponentWindow;
    LevelWindow m_LevelWindow;
    ToolWindow m_ToolWindow;
    EditorWindow m_EditorWindow;

    bool m_EditingName;
};

#endif // !_EGGEDITOR_H_