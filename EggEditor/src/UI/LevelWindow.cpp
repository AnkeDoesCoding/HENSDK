#include "LevelWindow.h"

void LevelWindow::Initialise(ComponentWindow* compWindow)
{
    m_CompWindow = compWindow;

    hen::graphics::TextureDesc iconDesc;
    iconDesc.Path = "res/ui/bricks.png";
    m_EntityIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

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

                auto* texture = hen::renderer::GetTextureManager()->Get(m_EntityIcon);

                ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(texture->GetID())), ImVec2(16, 16));
                ImGui::SameLine();

                if (ImGui::Selectable(entity.GetComponent<hen::level::NameComponent>().Name.c_str(), isSelected))
                {
                    m_CompWindow->SelectedEntity = entity;
                }
            }
        }

        ImGui::End();

        ImGui::Begin("Level", nullptr, ImGuiWindowFlags_NoCollapse);

        if (auto level = hen::level::GetActiveLevel())
        {
            ImGui::DragFloat3("Gravity", &level->Gravity.x, 0.01f);
        }
        
        ImGui::End();
    });
}