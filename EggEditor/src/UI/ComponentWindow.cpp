#include "ComponentWindow.h"

void ComponentWindow::Initialise()
{
    hen::ui::GetIMGUIManager()->RegisterDrawCallback([&]() 
    {
        ImGui::Begin("Components");

        if (SelectedEntity)
        {
            if (SelectedEntity.HasComponent<hen::level::TransformComponent>())
            {
                auto& transform = SelectedEntity.GetComponent<hen::level::TransformComponent>();

                glm::vec3 rotationDeg = glm::degrees(transform.Rotation);  

                ImGui::Text("Transform");

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::DragFloat3("Position", &transform.Position.x, 0.1f);

                ImGui::SameLine();
                if (ImGui::Button("Reset##pos"))
                {
                    transform.Position = glm::vec3(0.0f);
                }

                ImGui::DragFloat3("Rotation", &rotationDeg.x, 0.5f);

                ImGui::SameLine();
                if (ImGui::Button("Reset##rot"))
                {
                    transform.Rotation = glm::vec3(0.0f);
                    rotationDeg = glm::degrees(transform.Rotation);  
                }

                ImGui::DragFloat3("Scale", &transform.Scale.x, 0.1f);

                ImGui::SameLine();
                if (ImGui::Button("Reset##scale"))
                {
                    transform.Scale = glm::vec3(1.0f);
                }

                transform.SetRotation(glm::radians(rotationDeg));

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Spacing();

            }
        
            if (SelectedEntity.HasComponent<hen::level::MeshComponent>())
            {
                auto& mesh = SelectedEntity.GetComponent<hen::level::MeshComponent>();
                
                ImGui::Text("Mesh");

                ImGui::Spacing();
                ImGui::Spacing();

                std::string verticesText = "Vertices: " + std::to_string(mesh.Vertices.size());
                std::string normalsText = "Normals: " + std::to_string(mesh.Normals.size());
                std::string indicesText = "Indices: " + std::to_string(mesh.Indices.size());
                std::string submeshText = "Submeshes: " + std::to_string(mesh.SubMeshes.size());

                ImGui::Text("%s", verticesText.c_str());
                ImGui::Text("%s", normalsText.c_str());
                ImGui::Text("%s", indicesText.c_str());
                ImGui::Text("%s", submeshText.c_str());

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Spacing();

            }
        
            if (SelectedEntity.HasComponent<hen::level::LightComponent>())
            {
                auto& light = SelectedEntity.GetComponent<hen::level::LightComponent>();

                ImGui::Text("Light");

                const char* lightTypes[] = { "Point", "Spot", "Directional" };
                int currentType = static_cast<int>(light.Type);

                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::Combo("Type", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes)))
                {
                    light.Type = static_cast<hen::level::LIGHT_TYPES>(currentType);
                }

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::DragFloat("Range", &light.Range, 0.1f, 0.0f);
                ImGui::DragFloat("Intensity", &light.Intensity, 0.1f, 0.0f);
                ImGui::ColorPicker3("Colour", &light.Colour.x);

                if (light.Type == hen::level::LIGHT_TYPES::SPOT)
                {
                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::DragFloat("Inner Cut Off", &light.InnerCutOff, 0.1f, 0.0f);
                    ImGui::DragFloat("Outer Cut Off", &light.OuterCutOff, 0.1f, 0.0f);
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Spacing();
            }
        }

        ImGui::End();
        
    });
}