#include "ComponentWindow.h"

void ComponentWindow::Initialise()
{
    hen::graphics::TextureDesc iconDesc;

    iconDesc.Path = "res/ui/arrow_undo.png";
    m_ResetIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/brick_delete.png";
    m_RemoveIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/brick_add.png";
    m_AddIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/arrow_switch.png";
    m_TransformIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/images.png";
    m_MaterialIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/sport_soccer.png";
    m_RigidBodyIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/package.png";
    m_MeshIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/lightning.png";
    m_LightIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    iconDesc.Path = "res/ui/tag_orange.png";
    m_NameIcon = hen::renderer::GetTextureManager()->Load(iconDesc);

    hen::ui::GetIMGUIManager()->RegisterDrawCallback([&]() 
    {
        ImGui::Begin("Components");

        if (SelectedEntity)
        {
            auto* removeTexture = hen::renderer::GetTextureManager()->Get(m_RemoveIcon);
            auto* addTexture = hen::renderer::GetTextureManager()->Get(m_AddIcon);
            auto *nameTexture = hen::renderer::GetTextureManager()->Get(m_NameIcon);

            auto &name = SelectedEntity.GetComponent<hen::level::NameComponent>();

            char nameBuffer[256];
            strncpy(nameBuffer, name.Name.c_str(), sizeof(nameBuffer));
            nameBuffer[sizeof(nameBuffer) - 1] = '\0';

            const char* components[] = {"Transform", "Material", "Mesh", "Light", "RigidBody"};
            int currentComponent = -1;

            ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(nameTexture->GetID())), ImVec2(16, 16));

            ImGui::SameLine();
            ImGui::SetWindowFontScale(1.1f);
            ImGui::Text("Name");
            ImGui::SetWindowFontScale(1.0f);

            ImGui::Spacing();
            ImGui::Spacing();

            if (ImGui::InputText("##name", nameBuffer, sizeof(nameBuffer)))
            {
                name.Name = nameBuffer;
            }

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(addTexture->GetID())), ImVec2(16, 16));
            
            ImGui::SameLine();
            ImGui::Text("Add Component:");

            if (ImGui::Combo("##components", &currentComponent, components, IM_ARRAYSIZE(components)))
            {
                switch (currentComponent)
                {
                case 0:
                    if (!SelectedEntity.HasComponent<hen::level::TransformComponent>())
                    {
                        SelectedEntity.AddComponent<hen::level::TransformComponent>();
                    }
                    break;
                case 1:
                    if (!SelectedEntity.HasComponent<hen::level::MaterialComponent>())
                    {
                        SelectedEntity.AddComponent<hen::level::MaterialComponent>();
                    }
                    break;
                case 2:
                    if (!SelectedEntity.HasComponent<hen::level::MeshComponent>())
                    {
                        SelectedEntity.AddComponent<hen::level::MeshComponent>();
                    }   
                    break;
                case 3:
                    if (!SelectedEntity.HasComponent<hen::level::LightComponent>())
                    {
                        SelectedEntity.AddComponent<hen::level::LightComponent>();
                    }
                    break;
                case 4:
                    if (!SelectedEntity.HasComponent<hen::level::RigidBodyComponent>())
                    {
                        SelectedEntity.AddComponent<hen::level::RigidBodyComponent>();
                    }
                    break;
                default:
                    break;
                }
            }

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();

            if (SelectedEntity.HasComponent<hen::level::TransformComponent>())
            {
                auto& transform = SelectedEntity.GetComponent<hen::level::TransformComponent>();

                hen::math::Vec3 rotation = transform.GetEulerRotation();

                auto* resetTexture = hen::renderer::GetTextureManager()->Get(m_ResetIcon);
                auto* transformTexture = hen::renderer::GetTextureManager()->Get(m_TransformIcon);

                ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(transformTexture->GetID())), ImVec2(16, 16));

                ImGui::SameLine();
                ImGui::SetWindowFontScale(1.1f);
                ImGui::Text("Transform Component");
                ImGui::SetWindowFontScale(1.0f);

                ImGui::SameLine(0.0f, 32.0f);
                if (ImGui::ImageButton("Remove##transform", static_cast<ImTextureID>(static_cast<intptr_t>(removeTexture->GetID())), ImVec2(16, 16)))
                {
                    SelectedEntity.RemoveComponent<hen::level::TransformComponent>();
                }

                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::DragFloat3("Position", &transform.LocalPosition.x, 0.1f))
                {
                    transform.SetDirty();
                }
                
                ImGui::SameLine();
                if (ImGui::ImageButton("Reset##pos", static_cast<ImTextureID>(static_cast<intptr_t>(resetTexture->GetID())), ImVec2(16, 16)))
                {
                    transform.LocalPosition = hen::math::Vec3(0.0f);
                    transform.SetDirty();

                }

                if (ImGui::DragFloat3("Rotation", &rotation.x, 0.5f))
                {
                    transform.SetEulerRotation(rotation);
                    transform.SetDirty();

                }

                ImGui::SameLine();
                if (ImGui::ImageButton("Reset##rot", static_cast<ImTextureID>(static_cast<intptr_t>(resetTexture->GetID())), ImVec2(16, 16)))
                {
                    transform.SetLocalRotation(hen::math::Quat(1,0,0,0));
                    rotation = transform.GetEulerRotation();  
                    transform.SetDirty();

                }

                if (ImGui::DragFloat3("Scale", &transform.LocalScale.x, 0.1f))
                {
                    transform.SetDirty();
                }

                ImGui::SameLine();
                 if (ImGui::ImageButton("Reset##scale", static_cast<ImTextureID>(static_cast<intptr_t>(resetTexture->GetID())), ImVec2(16, 16)))
                {
                    transform.LocalScale = hen::math::Vec3(1.0f);
                    transform.SetDirty();
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Spacing();

            }
        
            if (SelectedEntity.HasComponent<hen::level::RigidBodyComponent>())
            {
                auto& rigidBody = SelectedEntity.GetComponent<hen::level::RigidBodyComponent>();

                const char* collisionTypes[] = { "Box", "Sphere", "Capsule", "Cylinder", "Convex Hull", "Triangle Mesh" };
                int currentType = static_cast<int>(rigidBody.Shape);

                auto* rbTexture = hen::renderer::GetTextureManager()->Get(m_RigidBodyIcon);

                ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(rbTexture->GetID())), ImVec2(16, 16));

                ImGui::SameLine();
                ImGui::SetWindowFontScale(1.1f);
                ImGui::Text("Rigid Body Component");
                ImGui::SetWindowFontScale(1.0f);

                ImGui::SameLine(0.0f, 32.0f);
                if (ImGui::ImageButton("Remove##rigidbody", static_cast<ImTextureID>(static_cast<intptr_t>(removeTexture->GetID())), ImVec2(16, 16)))
                {
                    SelectedEntity.RemoveComponent<hen::level::RigidBodyComponent>();
                }

                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::Combo("Collision Shape", &currentType, collisionTypes, IM_ARRAYSIZE(collisionTypes)))
                {
                    rigidBody.Shape = static_cast<hen::level::COLLISION_SHAPES>(currentType);
                    rigidBody.SetDirty();
                }

                switch (rigidBody.Shape)
                {
                    case hen::level::COLLISION_SHAPES::BOX:
                        ImGui::Spacing();
                        ImGui::Spacing();

                        if (ImGui::DragFloat3("Box Half Extents", &rigidBody.Box.HalfExtents.x, 0.1f))
                        {
                            rigidBody.SetDirty();
                        }

                        ImGui::Spacing();
                        ImGui::Spacing();
                        break;
                    case hen::level::COLLISION_SHAPES::SPHERE:
                        ImGui::Spacing();
                        ImGui::Spacing();

                        if (ImGui::InputFloat("Sphere Radius", &rigidBody.Sphere.Radius))
                        {
                            rigidBody.SetDirty();
                        }

                        ImGui::Spacing();
                        ImGui::Spacing();
                        break;
                    case hen::level::COLLISION_SHAPES::CAPSULE:
                        ImGui::Spacing();
                        ImGui::Spacing();

                        if (ImGui::InputFloat("Capsule Radius", &rigidBody.Capsule.Radius))
                        {
                            rigidBody.SetDirty();
                        }

                        if (ImGui::InputFloat("Capsule Height", &rigidBody.Capsule.Height))
                        {
                            rigidBody.SetDirty();
                        }

                        ImGui::Spacing();
                        ImGui::Spacing();
                        break;
                    case hen::level::COLLISION_SHAPES::CYLINDER:
                        ImGui::Spacing();
                        ImGui::Spacing();

                        if (ImGui::InputFloat("Cylinder Radius", &rigidBody.Cylinder.Radius))
                        {
                            rigidBody.SetDirty();
                        }

                        if (ImGui::InputFloat("Cylinder Height", &rigidBody.Cylinder.Height))
                        {
                            rigidBody.SetDirty();
                        }

                        ImGui::Spacing();
                        ImGui::Spacing();
                        break;
                    default:
                        ImGui::Spacing();
                        ImGui::Spacing();
                        break;
                }

                ImGui::InputFloat("Mass", &rigidBody.Mass);
                ImGui::InputFloat("Friction", &rigidBody.Friction);
                ImGui::InputFloat("Restituiton", &rigidBody.Restitution);

                if (ImGui::InputFloat("LinearDamping", &rigidBody.LinearDamping))
                {
                    rigidBody.SetDirty();
                }

                if (ImGui::InputFloat("AngularDamping", &rigidBody.AngularDamping))
                {
                    rigidBody.SetDirty();
                }

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::Checkbox("Kinematic", &rigidBody.Kinematic);
                ImGui::Checkbox("DisableDeactivation", &rigidBody.DisableDeactivation);
                ImGui::Checkbox("StartDeactivated", &rigidBody.StartDeactivated);

                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::InputFloat3("Offset", &rigidBody.Offset.x))
                {
                    rigidBody.SetDirty();
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
                
                auto* meshTexture = hen::renderer::GetTextureManager()->Get(m_MeshIcon);

                ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(meshTexture->GetID())), ImVec2(16, 16));

                ImGui::SameLine();
                ImGui::SetWindowFontScale(1.1f);
                ImGui::Text("Mesh Component");
                ImGui::SetWindowFontScale(1.0f);

                ImGui::SameLine(0.0f, 32.0f);
                if (ImGui::ImageButton("Remove##mesh", static_cast<ImTextureID>(static_cast<intptr_t>(removeTexture->GetID())), ImVec2(16, 16)))
                {
                    SelectedEntity.RemoveComponent<hen::level::MeshComponent>();
                }

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

            if (SelectedEntity.HasComponent<hen::level::MaterialComponent>())
            {
                auto& material = SelectedEntity.GetComponent<hen::level::MaterialComponent>();
                
                auto* materialTexture = hen::renderer::GetTextureManager()->Get(m_MaterialIcon);

                ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(materialTexture->GetID())), ImVec2(16, 16));

                ImGui::SameLine();
                ImGui::SetWindowFontScale(1.1f);
                ImGui::Text("Material Component");
                ImGui::SetWindowFontScale(1.0f);

                ImGui::SameLine(0.0f, 32.0f);
                if (ImGui::ImageButton("Remove##texture", static_cast<ImTextureID>(static_cast<intptr_t>(removeTexture->GetID())), ImVec2(16, 16)))
                {
                    SelectedEntity.RemoveComponent<hen::level::MaterialComponent>();
                }

                ImGui::Spacing();
                ImGui::Spacing();

                std::string diffuseText = "Diffuse Textures: " + std::to_string(material.DiffuseTextures.size());
                std::string specularText = "Specular Textures: " + std::to_string(material.SpecularTextures.size());

                ImGui::Text("%s", diffuseText.c_str());
                ImGui::Text("%s", specularText.c_str());

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::Text("Colour");
                ImGui::ColorPicker3("", &material.Colour.x);

                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::TreeNode("Diffuse Textures"))
                {
                    for (size_t i = 0; i < material.DiffuseTextures.size(); ++i)
                    {
                        if (auto* texture = hen::renderer::GetTextureManager()->Get(material.DiffuseTextures[i]))
                        {
                            ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(texture->GetID())), ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1));

                            ImGui::SameLine();
                            ImGui::BeginGroup();
                            {
                                ImGui::Text("Index: %zu", i);

                                std::string buttonLabel = "Remove##difftexture_" + std::to_string(i);
                                if (ImGui::Button(buttonLabel.c_str()))
                                {
                                    hen::renderer::GetTextureManager()->Release(material.DiffuseTextures[i]);
                                    material.DiffuseTextures.erase(material.DiffuseTextures.begin() + i);

                                    if (SelectedEntity.HasComponent<hen::level::MeshComponent>())
                                    {
                                        auto &mesh = SelectedEntity.GetComponent<hen::level::MeshComponent>();

                                        for (auto &submesh : mesh.SubMeshes)
                                        {
                                            if (submesh.DiffuseIndex > i)
                                            {
                                                submesh.DiffuseIndex--;
                                            }
                                            else if (submesh.DiffuseIndex == i)
                                            {
                                                submesh.DiffuseIndex = UINT32_MAX;
                                            }
                                        }
                                    }

                                    ImGui::EndGroup();
                                    break;
                                }
                            }
                            ImGui::EndGroup();
                        }
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Specular Textures"))
                {
                    for (size_t i = 0; i < material.SpecularTextures.size(); ++i)
                    {
                        if (auto *texture = hen::renderer::GetTextureManager()->Get(material.SpecularTextures[i]))
                        {
                            ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(texture->GetID())), ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1));

                            ImGui::SameLine();
                            ImGui::BeginGroup();
                            {
                                ImGui::Text("Index: %zu", i);

                                std::string buttonLabel = "Remove##spectexture_" + std::to_string(i);
                                if (ImGui::Button(buttonLabel.c_str()))
                                {
                                    hen::renderer::GetTextureManager()->Release(material.SpecularTextures[i]);
                                    material.SpecularTextures.erase(material.SpecularTextures.begin() + i);

                                    if (SelectedEntity.HasComponent<hen::level::MeshComponent>())
                                    {
                                        auto &mesh = SelectedEntity.GetComponent<hen::level::MeshComponent>();

                                        for (auto &submesh : mesh.SubMeshes)
                                        {
                                            if (submesh.SpecularIndex > i)
                                            {
                                                submesh.SpecularIndex--;
                                            }
                                            else if (submesh.SpecularIndex == i)
                                            {
                                                submesh.SpecularIndex = UINT32_MAX;
                                            }
                                        }
                                    }
                                    
                                    ImGui::EndGroup();
                                    break;
                                }
                            }
                            ImGui::EndGroup();
                        }
                    }
                    ImGui::TreePop();
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Spacing();
            }
        
            if (SelectedEntity.HasComponent<hen::level::LightComponent>())
            {
                auto& light = SelectedEntity.GetComponent<hen::level::LightComponent>();

                auto* lightTexture = hen::renderer::GetTextureManager()->Get(m_LightIcon);

                ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(lightTexture->GetID())), ImVec2(16, 16));

                ImGui::SameLine();
                ImGui::SetWindowFontScale(1.1f);
                ImGui::Text("Light Component");
                ImGui::SetWindowFontScale(1.0f);

                ImGui::SameLine(0.0f, 32.0f);
                if (ImGui::ImageButton("Remove##light", static_cast<ImTextureID>(static_cast<intptr_t>(removeTexture->GetID())), ImVec2(16, 16)))
                {
                    SelectedEntity.RemoveComponent<hen::level::LightComponent>();
                }

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