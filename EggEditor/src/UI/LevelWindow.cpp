#include "LevelWindow.h"

void LevelWindow::Initialise(ComponentWindow* compWindow)
{
    m_CompWindow = compWindow;

    hen::ui::GetIMGUIManager()->RegisterDrawCallback([&]() 
    {
        ImGui::Begin("Entities");

        if (auto level = hen::level::GetActiveLevel())
        {
            if (m_CompWindow == nullptr)
            {
                return;
            }
            
            auto view = level->GetView<hen::level::NameComponent>(); // every entity has a name component

            for (auto entityHandle : view)
            {
                hen::level::Entity entity(entityHandle, level);

                bool isSelected = (m_CompWindow->SelectedEntity == entity);

                if (ImGui::Selectable(entity.GetComponent<hen::level::NameComponent>().Name.c_str(), isSelected))
                {
                    m_CompWindow->SelectedEntity = entity;
                }
            }
        }

        ImGui::End();

        ImGui::Begin("Level");

        if (auto level = hen::level::GetActiveLevel())
        {
            ImGui::DragFloat3("Gravity", &level->Gravity.x, 0.01f);
        }
        
        ImGui::End();
    });
}