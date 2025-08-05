#include "EggEditor.h"

static hen::scene::actors::Camera& Cam = hen::renderer::Camera;
static float MouseSensitivity = 3.0f;

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
}

void Editor::Update(float deltaTime) 
{
    hen::Application::Update(deltaTime);

    float vel = Cam.Speed * deltaTime;

    if(hen::input::Down(hen::input::BUTTON('W')))
    {
        Cam.Position += Cam.Front * vel;
    }
    if(hen::input::Down(hen::input::BUTTON('S')))
    {
        Cam.Position -= Cam.Front * vel;            
    }
    if(hen::input::Down(hen::input::BUTTON('A')))
    {
        Cam.Position -= Cam.Right * vel;            
    }
    if(hen::input::Down(hen::input::BUTTON('D')))
    {
        Cam.Position += Cam.Right * vel;            
    }

    float xDiff, yDiff;

    glm::vec2 originalMouse = glm::vec2(0.0f, 0.0f);
    glm::vec2 currentMouse = hen::input::GetPointerPos();

    xDiff = hen::input::GetMouseState().DeltaPos.x;
    yDiff = hen::input::GetMouseState().DeltaPos.y;
    xDiff = xDiff * (1.0f / 60.0f);
	yDiff = yDiff * (1.0f / 60.0f);

    Cam.Yaw += xDiff * MouseSensitivity;
    Cam.Pitch -= yDiff * MouseSensitivity;
    Cam.Speed += hen::input::GetMouseState().DeltaWheel * 2;

    if(Cam.Speed <= 0.0f)
    {
        Cam.Speed = 1.0f;
    }
    
}