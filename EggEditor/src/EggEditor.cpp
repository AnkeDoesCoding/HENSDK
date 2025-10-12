#include "EggEditor.h"

#include "ModelImporter.h"

static hen::level::CameraComponent& Cam = hen::renderer::Camera;
static float MouseSensitivity = 4.0f;
static bool MouseLocked = false;
static float CameraVelocity = 0.0f;
static float CameraSpeed = 1.0f;

static hen::level::Entity* Test;


void Editor::Initialise(SDL_Window* window)
{
    hen::Application::Initialise(window);

    Test = new hen::level::Entity(hen::level::GetActiveLevel()->CreateEntity("test"));
    auto& transform = Test->AddComponent<hen::level::TransformComponent>();
    transform.SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    transform.SetScale(glm::vec3(0.5f));
    auto& mesh = Test->AddComponent<hen::level::MeshComponent>();

    importer::ImportModel("res/models/survival_guitar_backpack/scene.gltf", mesh);

    hen::ui::GetIMGUIManager()->RegisterDrawCallback([]() 
    {
        ImGui::Begin("Info");
        
        if(ImGui::CollapsingHeader("Controls"))
        {
            ImGui::Text("W,A,S,D - move around");
            ImGui::Text("M - toggle mouse lock");
            ImGui::Text("Tilde - toggle console");
            ImGui::Text("Esc - shutdown application");
        }
    
        if(ImGui::CollapsingHeader("Stats"))
        {
            ImGui::Text("FPS:  %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("MS:  %.3f", 1000.0f / ImGui::GetIO().Framerate);
        }
    
        if(ImGui::CollapsingHeader("Camera"))
        {
            ImGui::Text("Speed:  %.1f", CameraSpeed);
            ImGui::Text("FOV:  %.0f", Cam.FOV);
            ImGui::Text("Yaw:  %.3f", Cam.Rotation.y);
            ImGui::Text("Pitch:  %.1f", Cam.Rotation.x);
            ImGui::Text("Position:  %.4f, %.4f, %.4f", Cam.Position.x, Cam.Position.y, Cam.Position.z);
        }
        ImGui::End();
    });
}

void Editor::Shutdown()
{
    hen::Application::Shutdown();

    Test = nullptr;
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

    if(MouseLocked && !hen::console::Visible)
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
        if(hen::input::Press(hen::input::BUTTON('M')))
        {
            MouseLocked = !MouseLocked;
            MouseLocked ? hen::input::LockMouse() : hen::input::UnLockMouse();
        }
    }
    
}