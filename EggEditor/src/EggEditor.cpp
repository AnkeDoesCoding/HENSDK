#include "EggEditor.h"

#include "ModelImporter.h"
#include "TestLevel.h"

static float MouseSensitivity = 4.0f;
static float CameraVelocity = 0.0f;
static float CameraSpeed = 1.0f;

static bool hasLoadedLevel = false;

void Editor::Initialise(SDL_Window* window)
{
    hen::Application::Initialise(window);

    m_ComponentWindow.Initialise();
    m_LevelWindow.Initialise(&m_ComponentWindow);
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

    CameraVelocity = CameraSpeed * deltaTime;

    float xDiff = 0.0f , yDiff = 0.0f;

    hen::math::Vec2 originalMouse = hen::math::Vec2(0.0f, 0.0f);
    hen::math::Vec2 currentMouse = hen::input::GetPointerPos();

    if (hen::input::GetMouseState().Locked && !hen::console::Visible)
    {
        xDiff = hen::input::GetMouseState().DeltaPos.x;
        yDiff = hen::input::GetMouseState().DeltaPos.y;
        xDiff = xDiff * (1.0f / 60.0f);
	    yDiff = yDiff * (1.0f / 60.0f);
    }

    hen::renderer::Camera.Rotation.y += xDiff * MouseSensitivity;
    hen::renderer::Camera.Rotation.x -= yDiff * MouseSensitivity;

    CameraSpeed += hen::input::GetMouseState().DeltaWheel * 2;

    if (CameraSpeed <= 0.0f)
    {
        CameraSpeed = 1.0f;
    }

    if (!hen::console::Visible)
    {
        if (hen::input::Down(hen::input::BUTTON('W')))
        {
            hen::renderer::Camera.Position += hen::renderer::Camera.Front * CameraVelocity;
        }
        if (hen::input::Down(hen::input::BUTTON('S')))
        {
            hen::renderer::Camera.Position -= hen::renderer::Camera.Front * CameraVelocity;            
        }
        if (hen::input::Down(hen::input::BUTTON('A')))
        {
            hen::renderer::Camera.Position -= hen::renderer::Camera.Right * CameraVelocity;            
        }
        if (hen::input::Down(hen::input::BUTTON('D')))
        {
            hen::renderer::Camera.Position += hen::renderer::Camera.Right * CameraVelocity;            
        }
        
        if (hen::input::Press(hen::input::BUTTON(hen::input::MOUSE_BUTTON_LEFT)) && hen::input::Down(hen::input::KEYBOARD_BUTTON_LCONTROL))
        {

            hen::level::Ray ray(hen::input::GetPointerPos(), 0.0f, 1000.0f);
            hen::level::RayResult result = hen::physics::CastRay(ray);

            if (result.HitEntity)
            {
                hen::physics::AddImpulseAt(result.HitEntity->GetComponent<hen::level::RigidBodyComponent>(), hen::renderer::Camera.Front * 1000.0f, result.HitPosition);
            }
        }
        

        if (hen::input::Down(hen::input::BUTTON(hen::input::MOUSE_BUTTON_RIGHT)))
        {
            hen::input::LockMouse();
        }
        else
        {
            hen::input::UnLockMouse();
        }
    }

    if (!hasLoadedLevel)
    {
        testlevel::Load();
        hasLoadedLevel = true;
    }
}