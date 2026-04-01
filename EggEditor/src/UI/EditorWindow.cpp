#include "EditorWindow.h"

void EditorWindow::Intialise()
{
    hen::graphics::TextureDesc iconDesc;

    iconDesc.Path = "res/ui/disk.png";
    m_SaveIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/arrow_undo.png";
    m_UndoIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/arrow_redo.png";
    m_RedoIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    hen::ui::GetIMGUIManager()->RegisterDrawCallback([&]() 
    {
        ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoScrollbar);

        auto* saveTexture = hen::renderer::GetTextureManager()->Get(m_SaveIcon);
        auto* undoTexture = hen::renderer::GetTextureManager()->Get(m_UndoIcon);
        auto* redoTexture = hen::renderer::GetTextureManager()->Get(m_RedoIcon);

        ImGuiDockNode* node = ImGui::GetWindowDockNode();
        if (node)
        {
            node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingSplit | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingOverEmpty;
        }

        if (ImGui::BeginTabBar("##tabs"))
        {
            if (ImGui::BeginTabItem("Editor"))
            {
                if (ImGui::ImageButton("Undo", static_cast<ImTextureID>(static_cast<intptr_t>(undoTexture->GetID())), ImVec2(16, 16)))
                {
                    // TODO: IMPLEMENT
                }

                ImGui::SameLine(0.0f, 8.0f);

                if (ImGui::ImageButton("Redo", static_cast<ImTextureID>(static_cast<intptr_t>(redoTexture->GetID())), ImVec2(16, 16)))
                {
                    // TODO: IMPLEMENT
                }

                ImGui::SameLine(0.0f, 16.0f);
                
                if (ImGui::Button("Controls"))
                {
                    m_OpenControlWindow = true;
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("File"))
            {
                ImGui::Button("Open");

                ImGui::SameLine(0.0f, 16.0f);

                ImGui::Button("Save");

                ImGui::SameLine(0.0f, 8.0f);

                ImGui::Button("Save as");

                ImGui::SameLine(0.0f, 16.0f);

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        if (m_OpenControlWindow)
        {
            ImGui::Begin("Controls", &m_OpenControlWindow);

            ImGui::SetWindowFontScale(1.1f);
            ImGui::Text("Navigation");
            ImGui::SetWindowFontScale(1.0f);

            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("W-A-S-D : Fly forward-left-right-backward");
            ImGui::Text("F : Focus on selected");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SetWindowFontScale(1.1f);
            ImGui::Text("Tools");
            ImGui::SetWindowFontScale(1.0f);

            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Left Control + Left Click : Apply impulse");
            ImGui::Text("F2 : Rename selected entity");

            ImGui::End();
        }
    });
}