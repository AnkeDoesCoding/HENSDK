#include "EggEditor.h"

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
    Test->AddComponent<hen::level::TransformComponent>();

    auto& mesh1 = Test->AddComponent<hen::level::MeshComponent>();

        mesh1.Verticies = 
        {
            {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
            {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f},
            {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f},
            { 0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f, -0.5f},
            {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f},
            {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}
        };

        mesh1.Normals = 
        {
            {0.0f, 0.0f, -1.0f},{0.0f, 0.0f, -1.0f},{0.0f, 0.0f, -1.0f},{0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f, 1.0f},{0.0f, 0.0f, 1.0f},{0.0f, 0.0f, 1.0f},{0.0f, 0.0f, 1.0f},
            {-1.0f, 0.0f, 0.0f},{-1.0f, 0.0f, 0.0f},{-1.0f, 0.0f, 0.0f},{-1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},{1.0f, 0.0f, 0.0f},{1.0f, 0.0f, 0.0f},{1.0f, 0.0f, 0.0f},
            {0.0f, -1.0f, 0.0f},{0.0f, -1.0f, 0.0f},{0.0f, -1.0f, 0.0f},{0.0f, -1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},{0.0f, 1.0f, 0.0f},{0.0f, 1.0f, 0.0f},{0.0f, 1.0f, 0.0f}
        };

        mesh1.Indicies = 
        {
            0,1,2, 2,3,0,
            4,5,6, 6,7,4,
            8,9,10, 10,11,8,
            12,13,14, 14,15,12,
            16,17,18, 18,19,16,
            20,21,22, 22,23,20
        };

        mesh1.CreateRenderData();
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

    auto& trans = Test->GetComponent<hen::level::TransformComponent>();

    glm::vec3 newPos = glm::vec3(Cam.Position.x, Cam.Position.y + 3, Cam.Position.z);
    trans.SetPosition(newPos);

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