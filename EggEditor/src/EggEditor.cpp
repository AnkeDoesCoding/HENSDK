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
    m_ToolWindow.Initialise(&m_ComponentWindow);
    m_EditorWindow.Intialise();

    hen::ui::GetIMGUIManager()->RegisterDrawCallback([&]() 
    {
        if (m_EditingName)
        {
            if (!m_ComponentWindow.SelectedEntity.IsValid())
            {
                return;
            }

            auto &name = m_ComponentWindow.SelectedEntity.GetComponent<hen::level::NameComponent>();

            char nameBuffer[256];
            strncpy(nameBuffer, name.Name.c_str(), sizeof(nameBuffer));
            nameBuffer[sizeof(nameBuffer) - 1] = '\0';

            int windowWidth, windowHeight;

            SDL_GetWindowSize(hen::renderer::GetRHC()->GetWindow(), &windowWidth, &windowHeight);

            ImGui::SetNextWindowSize(ImVec2(130.0f, 65.0f));
            ImGui::SetNextWindowPos(ImVec2(windowWidth / 2 - 130.0f, windowHeight / 2 - 65.0f));

            ImGui::Begin("Edit name", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

            ImGui::SetKeyboardFocusHere();

            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputText("##name", nameBuffer, sizeof(nameBuffer)))
            {
                name.Name = nameBuffer;
            }

            if (hen::input::Press(hen::input::KEYBOARD_BUTTON_ESCAPE) || hen::input::Press(hen::input::KEYBOARD_BUTTON_ENTER))
            {
                m_EditingName = false;
            }

            ImGui::End();
        }
    });
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
                hen::math::Vec3 dir(result.HitPosition - hen::renderer::Camera.Position);
                dir = hen::math::Normalise(dir);

                hen::physics::AddImpulseAt(result.HitEntity->GetComponent<hen::level::RigidBodyComponent>(), dir * 100.0f, result.HitPosition);
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

    if (hen::input::Press(hen::input::KEYBOARD_BUTTON_F2))
    {
        m_EditingName = true;
    }

    if (hen::input::Press(hen::input::BUTTON('F')))
    {
        if (m_ComponentWindow.SelectedEntity.IsValid())
        {
            if (m_ComponentWindow.SelectedEntity.HasComponent<hen::level::TransformComponent>())
            {
                auto& transform = m_ComponentWindow.SelectedEntity.GetComponent<hen::level::TransformComponent>();

                hen::renderer::Camera.Position = (transform.LocalPosition + hen::math::Vec3(10.0f, 5.0f, 10.0f));

                hen::renderer::Camera.LookAt(transform.LocalPosition);
            }
        }
    }

    int windowWidth, windowHeight;
    SDL_GetWindowSize(hen::renderer::GetRHC()->GetWindow(), &windowWidth, &windowHeight);

    ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(hen::ui::GetIMGUIManager()->GetDockSpaceID());

    if (!node)
    {
        hen::renderer::GetRHC()->ResizeViewport(0, 0, windowWidth, windowHeight);
        return;
    }

    hen::renderer::GetRHC()->ResizeViewport(node->Pos.x, windowHeight - node->Pos.y - node->Size.y, node->Size.x, node->Size.y);

    
    if (!hasLoadedLevel)
    {
        testlevel::Load();
        hasLoadedLevel = true;
    }
}