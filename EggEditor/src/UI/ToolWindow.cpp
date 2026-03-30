#include "ToolWindow.h"

void ToolWindow::Initialise(ComponentWindow* compWindow)
{
    m_ComponentWindow = compWindow;

    m_TranslateIcon = hen::renderer::GetTextureManager()->Load("res/ui/arrow_branch.png");
    m_RotateIcon = hen::renderer::GetTextureManager()->Load("res/ui/arrow_refresh.png");
    m_ScaleIcon = hen::renderer::GetTextureManager()->Load("res/ui/arrow_out.png");
    m_AddIcon = hen::renderer::GetTextureManager()->Load("res/ui/add.png");
    m_RemoveIcon = hen::renderer::GetTextureManager()->Load("res/ui/cancel.png");

    hen::ui::GetIMGUIManager()->RegisterDrawCallback([&]()
    {
        ImGui::Begin("Tools");

        ImGuiDockNode* node = ImGui::GetWindowDockNode();
        if (node)
        {
            node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoDockingSplit | ImGuiDockNodeFlags_NoDockingOverMe;
        }

        auto* translateTexture = hen::renderer::GetTextureManager()->Get(m_TranslateIcon);
        auto* rotateTexture = hen::renderer::GetTextureManager()->Get(m_RotateIcon);
        auto* scaleTexture = hen::renderer::GetTextureManager()->Get(m_ScaleIcon);
        auto* addTexture = hen::renderer::GetTextureManager()->Get(m_AddIcon);
        auto* removeTexture = hen::renderer::GetTextureManager()->Get(m_RemoveIcon);

        ImGui::Spacing();

        if (ImGui::ImageButton("Translate", static_cast<ImTextureID>(static_cast<intptr_t>(translateTexture->GetID())), ImVec2(24, 24)))
        {
            // TODO: IMPLEMENT IMGUIZMO
        }

        ImGui::Spacing();

        if (ImGui::ImageButton("Rotate", static_cast<ImTextureID>(static_cast<intptr_t>(rotateTexture->GetID())), ImVec2(24, 24)))
        {
            // TODO: IMPLEMENT IMGUIZMO
        }    

        ImGui::Spacing();

        if (ImGui::ImageButton("Scale", static_cast<ImTextureID>(static_cast<intptr_t>(scaleTexture->GetID())), ImVec2(24, 24)))
        {
            // TODO: IMPLEMENT IMGUIZMO
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::ImageButton("Add", static_cast<ImTextureID>(static_cast<intptr_t>(addTexture->GetID())), ImVec2(24, 24)))
        {
            if (auto level = hen::level::GetActiveLevel())
            {
                level->CreateEntity("New Entity");
            }
        }

        ImGui::Spacing();

        if (ImGui::ImageButton("Remove", static_cast<ImTextureID>(static_cast<intptr_t>(removeTexture->GetID())), ImVec2(24, 24)))
        {
            if (m_ComponentWindow->SelectedEntity)
            {
                hen::level::GetActiveLevel()->RemoveEntity(m_ComponentWindow->SelectedEntity);
            }
        }

        ImGui::End();
    });
}