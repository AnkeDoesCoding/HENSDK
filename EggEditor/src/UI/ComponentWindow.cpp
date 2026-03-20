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
            }

            if (SelectedEntity.HasComponent<hen::level::TransformComponent>())
            {
                auto& transform = SelectedEntity.GetComponent<hen::level::TransformComponent>();

                hen::math::Vec3 rotation = transform.GetEulerRotation();

                ImGui::Text("Transform");

                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::DragFloat3("Position", &transform.LocalPosition.x, 0.1f))
                {
                    transform.SetDirty();
                }

                ImGui::SameLine();
                if (ImGui::Button("Reset##pos"))
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
                if (ImGui::Button("Reset##rot"))
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
                if (ImGui::Button("Reset##scale"))
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

                ImGui::Text("Rigid Body");

                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::Combo("Collision Shape", &currentType, collisionTypes, IM_ARRAYSIZE(collisionTypes)))
                {
                    rigidBody.Shape = static_cast<hen::level::COLLISIONSHAPES>(currentType);
                    rigidBody.SetDirty();
                }

                ImGui::Spacing();
                ImGui::Spacing();

                switch (rigidBody.Shape)
                {
                    case hen::level::COLLISIONSHAPES::BOX:
                        if (ImGui::DragFloat3("Box Half Extents", &rigidBody.Box.HalfExtents.x, 0.1f))
                        {
                            rigidBody.SetDirty();
                        }
                        break;
                    case hen::level::COLLISIONSHAPES::SPHERE:
                        if (ImGui::InputFloat("Sphere Radius", &rigidBody.Sphere.Radius))
                        {
                            rigidBody.SetDirty();
                        }
                        break;
                    case hen::level::COLLISIONSHAPES::CAPSULE:
                        if (ImGui::InputFloat("Capsule Radius", &rigidBody.Capsule.Radius))
                        {
                            rigidBody.SetDirty();
                        }

                        if (ImGui::InputFloat("Capsule Height", &rigidBody.Capsule.Height))
                        {
                            rigidBody.SetDirty();
                        }
                        break;
                    case hen::level::COLLISIONSHAPES::CYLINDER:
                        if (ImGui::InputFloat("Cylinder Radius", &rigidBody.Cylinder.Radius))
                        {
                            rigidBody.SetDirty();
                        }

                        if (ImGui::InputFloat("Cylinder Height", &rigidBody.Cylinder.Height))
                        {
                            rigidBody.SetDirty();
                        }
                        break;
                    default:
                        break;
                }

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::InputFloat("Mass", &rigidBody.Mass);
                ImGui::InputFloat("Friction", &rigidBody.Friction);
                ImGui::InputFloat("Restituiton", &rigidBody.Restitution);
                ImGui::InputFloat("LinearDamping", &rigidBody.LinearDamping);
                ImGui::InputFloat("AngularDamping", &rigidBody.AngularDamping);

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

            if (SelectedEntity.HasComponent<hen::level::MaterialComponent>())
            {
                auto& material = SelectedEntity.GetComponent<hen::level::MaterialComponent>();
                
                ImGui::Text("Material");

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
                        if (auto *texture = hen::renderer::GetTextureManager()->Get(material.DiffuseTextures[i]))
                        {
                            ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(texture->GetID())), ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1));

                            ImGui::SameLine();
                            ImGui::BeginGroup();
                            {
                                ImGui::Text("Index: %zu", i);

                                std::string buttonLabel = "Remove##difftexture_" + std::to_string(i);
                                if (ImGui::Button(buttonLabel.c_str()))
                                {
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