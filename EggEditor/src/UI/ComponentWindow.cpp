#include "ComponentWindow.h"

void ComponentWindow::Initialise()
{
    hen::ui::GetIMGUIManager()->RegisterDrawCallback([&]() 
    {
        ImGui::Begin("Components");

        if (SelectedEntity)
        {
            if (m_LastSelectedEntity != SelectedEntity)
            {
                m_LastSelectedEntity = SelectedEntity;
                m_RotationInitialised = false;
            }

            if (SelectedEntity.HasComponent<hen::level::TransformComponent>())
            {
                auto& transform = SelectedEntity.GetComponent<hen::level::TransformComponent>();

                bool changed = false;
                
                if (!m_RotationInitialised)
                {
                    m_DegreesRotation = transform.GetEulerRotation();  
                    m_RotationInitialised = true;
                }

                ImGui::Text("Transform");

                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::DragFloat3("Position", &transform.LocalPosition.x, 0.1f))
                {
                    changed = true;
                }

                ImGui::SameLine();
                if (ImGui::Button("Reset##pos"))
                {
                    transform.LocalPosition = hen::math::Vec3(0.0f);
                }

                if (ImGui::DragFloat3("Rotation", &m_DegreesRotation.x, 0.5f))
                {
                    transform.SetEulerRotation(m_DegreesRotation);
                    changed = true;
                }

                ImGui::SameLine();
                if (ImGui::Button("Reset##rot"))
                {
                    transform.SetLocalRotation(hen::math::Quat(1,0,0,0));
                    m_DegreesRotation = transform.GetEulerRotation();  
                    changed = true;
                }

                if (ImGui::DragFloat3("Scale", &transform.LocalScale.x, 0.1f))
                {
                    changed = true;
                }

                ImGui::SameLine();
                if (ImGui::Button("Reset##scale"))
                {
                    transform.LocalScale = hen::math::Vec3(1.0f);
                }

                if (changed)
                {
                    transform.SetDirty();
                }

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
                std::string materialText = "Materials: " + std::to_string(mesh.Materials.size());

                ImGui::Text("%s", verticesText.c_str());
                ImGui::Text("%s", normalsText.c_str());
                ImGui::Text("%s", indicesText.c_str());
                ImGui::Text("%s", submeshText.c_str());
                ImGui::Text("%s", materialText.c_str());

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