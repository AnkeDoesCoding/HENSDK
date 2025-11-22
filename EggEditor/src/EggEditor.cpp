#include "EggEditor.h"

#include "ModelImporter.h"

static hen::level::CameraComponent& Cam = hen::renderer::Camera;
static float MouseSensitivity = 4.0f;
static float CameraVelocity = 0.0f;
static float CameraSpeed = 1.0f;

static hen::level::Entity* ModelEnt;
static hen::level::Entity* LightEnt;
static hen::level::Entity* LightEnt2;

static hen::level::Entity selectedEntity;

void Editor::Initialise(SDL_Window* window)
{
    hen::Application::Initialise(window);

    ModelEnt = new hen::level::Entity(hen::level::GetActiveLevel()->CreateEntity("model"));

    auto& transform = ModelEnt->AddComponent<hen::level::TransformComponent>();
    auto& mat = ModelEnt->AddComponent<hen::level::MaterialComponent>();
    auto& mesh = ModelEnt->AddComponent<hen::level::MeshComponent>();

    transform.SetScale(glm::vec3(0.15f));

    importer::ImportModel(ENGINE_RESOURCE_PATH "models/sponza/sponza.obj", mesh);

    mat.Shader = hen::renderer::GetShaderManager()->Load(ENGINE_RESOURCE_PATH "shaders/GLSL/BaseShaderVS.glsl", ENGINE_RESOURCE_PATH "shaders/GLSL/BaseShaderFS.glsl");

    LightEnt = new hen::level::Entity(hen::level::GetActiveLevel()->CreateEntity("light"));

    auto& transformLight = LightEnt->AddComponent<hen::level::TransformComponent>();
    auto& light = LightEnt->AddComponent<hen::level::LightComponent>();

    transformLight.SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));

    light.Range = 100.0f;
    light.Intensity = 1.0f;
    light.Type = hen::level::LIGHT_TYPES::POINT;

    LightEnt2 = new hen::level::Entity(hen::level::GetActiveLevel()->CreateEntity("light2"));

    auto& transformLight2 = LightEnt2->AddComponent<hen::level::TransformComponent>();
    auto& light2 = LightEnt2->AddComponent<hen::level::LightComponent>();

    light2.Range = 400.0f;
    light2.Intensity = 1.0f;
    light2.InnerCutOff = 20.0f;
    light2.OuterCutOff = 30.0f;
    light2.Type = hen::level::LIGHT_TYPES::SPOT;

    hen::ui::GetIMGUIManager()->RegisterDrawCallback([]() 
    {
        ImGui::Begin("Level");

        if (auto level = hen::level::GetActiveLevel())
        {
            auto view = level->GetView<hen::level::NameComponent>(); // every entity has a name component


            for (auto entityHandle : view)
            {
                hen::level::Entity entity(entityHandle, level);

                bool isSelected = (selectedEntity == entity);


                std::string label = entity.GetComponent<hen::level::NameComponent>().Name;

                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    selectedEntity = entity;
                }
            }
        }

        ImGui::End();

        ImGui::Begin("Components");

        if (selectedEntity)
        {
            if (selectedEntity.HasComponent<hen::level::TransformComponent>())
            {
                auto& transform = selectedEntity.GetComponent<hen::level::TransformComponent>();

                glm::vec3 rotationDeg = glm::degrees(transform.Rotation);  

                ImGui::Text("Transform");

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::DragFloat3("Position", &transform.Position.x, 0.1f);
                ImGui::DragFloat3("Rotation", &rotationDeg.x, 0.5f);
                ImGui::DragFloat3("Scale", &transform.Scale.x, 0.1f);

                transform.SetRotation(glm::radians(rotationDeg));

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Spacing();

            }
        
            if (selectedEntity.HasComponent<hen::level::MeshComponent>())
            {
                auto& mesh = selectedEntity.GetComponent<hen::level::MeshComponent>();
                
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
        
            if (selectedEntity.HasComponent<hen::level::LightComponent>())
            {
                auto& light = selectedEntity.GetComponent<hen::level::LightComponent>();

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

                
            }
        }

        ImGui::End();
        
    });
}

void Editor::Shutdown()
{
    hen::Application::Shutdown();

    free(ModelEnt);
    free(LightEnt);
    free(LightEnt2);
}

void Editor::FixedUpdate()
{
    hen::Application::FixedUpdate();
}

void Editor::Update(float deltaTime) 
{
    hen::Application::Update(deltaTime);

    CameraVelocity = CameraSpeed * deltaTime;

    float xDiff = 0.0f , yDiff = 0.0f;

    glm::vec2 originalMouse = glm::vec2(0.0f, 0.0f);
    glm::vec2 currentMouse = hen::input::GetPointerPos();

    if(hen::input::GetMouseState().Locked && !hen::console::Visible)
    {
        xDiff = hen::input::GetMouseState().DeltaPos.x;
        yDiff = hen::input::GetMouseState().DeltaPos.y;
        xDiff = xDiff * (1.0f / 60.0f);
	    yDiff = yDiff * (1.0f / 60.0f);
    }

    Cam.Rotation.y += xDiff * MouseSensitivity;
    Cam.Rotation.x -= yDiff * MouseSensitivity;

    CameraSpeed += hen::input::GetMouseState().DeltaWheel * 2;

    if(CameraSpeed <= 0.0f)
    {
        CameraSpeed = 1.0f;
    }

    if(!hen::console::Visible)
    {
    
        if(hen::input::Down(hen::input::BUTTON('W')))
        {
            Cam.Position += Cam.Front * CameraVelocity;
        }
        if(hen::input::Down(hen::input::BUTTON('S')))
        {
            Cam.Position -= Cam.Front * CameraVelocity;            
        }
        if(hen::input::Down(hen::input::BUTTON('A')))
        {
            Cam.Position -= Cam.Right * CameraVelocity;            
        }
        if(hen::input::Down(hen::input::BUTTON('D')))
        {
            Cam.Position += Cam.Right * CameraVelocity;            
        }
        if(hen::input::Down(hen::input::BUTTON(hen::input::MOUSE_BUTTON_RIGHT)))
        {
            hen::input::LockMouse();
        }
        else
        {
            hen::input::UnLockMouse();
        }
    }

    glm::vec3 rot(asin(Cam.Front.y), atan2(Cam.Front.z, Cam.Front.x), 0.0f);

    LightEnt2->GetComponent<hen::level::TransformComponent>().SetPosition(Cam.Position);
    LightEnt2->GetComponent<hen::level::TransformComponent>().SetRotation(rot);

}