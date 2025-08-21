#include "EggEditor.h"

static hen::scene::actors::Camera& Cam = hen::renderer::Camera;
static float MouseSensitivity = 4.0f;
static bool MouseLocked = false;
static float CameraVelocity = 0.0f;

void Editor::Initialise(SDL_Window* window)
{
    hen::Application::Initialise(window);
}

void Editor::Shutdown()
{
    hen::Application::Shutdown();
}

void Editor::FixedUpdate()
{
    hen::Application::FixedUpdate();

    float xDiff = 0.0f , yDiff = 0.0f;

    glm::vec2 originalMouse = glm::vec2(0.0f, 0.0f);
    glm::vec2 currentMouse = hen::input::GetPointerPos();

    if(MouseLocked)
    {
        xDiff = hen::input::GetMouseState().DeltaPos.x;
        yDiff = hen::input::GetMouseState().DeltaPos.y;
        xDiff = xDiff * (1.0f / 60.0f);
	    yDiff = yDiff * (1.0f / 60.0f);
    }

    Cam.Yaw += xDiff * MouseSensitivity;
    Cam.Pitch -= yDiff * MouseSensitivity;
    Cam.Speed += hen::input::GetMouseState().DeltaWheel * 2;

    if(Cam.Speed <= 0.0f)
    {
        Cam.Speed = 1.0f;
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
            MouseLocked = (MouseLocked ? false : true);
            (MouseLocked ? hen::input::LockMouse() : hen::input::UnLockMouse());
        }
    }

}

void Editor::Update(float deltaTime) 
{
    hen::Application::Update(deltaTime);

    CameraVelocity = Cam.Speed * deltaTime;
    
}