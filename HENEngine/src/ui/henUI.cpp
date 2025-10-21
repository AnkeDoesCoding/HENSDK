#include "ui/henUI.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/backends/imgui_impl_sdl3.h"
#include "vendor/imgui/backends/imgui_impl_opengl3.h"

#include "renderer/henRenderer.h"

namespace hen::ui
{
    void IMGUIManager::Initialise(SDL_Window* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                break;
            case renderer::BACKEND::OPENGL:
                ImGui_ImplSDL3_InitForOpenGL(window, SDL_GL_GetCurrentContext());
                ImGui_ImplOpenGL3_Init("#version 460");
                break;
            case renderer::BACKEND::VULKAN:
                break;
            default:
                break;
        }
        
        Initialised = true;
    }

    void IMGUIManager::Shutdown()
    {
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                break;
            case renderer::BACKEND::OPENGL:
                ImGui_ImplOpenGL3_Shutdown();
                break;
            case renderer::BACKEND::VULKAN:
                break;
            default:
                break;
        }

        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

    }

    void IMGUIManager::BeginFrame()
    {
        if (Initialised)
        {
            ImGui_ImplSDL3_NewFrame();
            switch (renderer::CurrentBackend)
            {
                case renderer::BACKEND::NONE:
                    break;
                case renderer::BACKEND::OPENGL:
                    ImGui_ImplOpenGL3_NewFrame();
                    break;
                case renderer::BACKEND::VULKAN:
                    break;
                default:
                    break;
            }
            ImGui::NewFrame();

            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_NoBackground;

            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
            ImGui::PopStyleVar(3);

            ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");
            ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::End();


            for (auto& callback : m_Callbacks)
            {
                callback();
            }
        }
    }

    void IMGUIManager::EndFrame()
    {
        if (Initialised)
        {
            ImGui::Render();

            switch (renderer::CurrentBackend)
            {
                case renderer::BACKEND::NONE:
                    break;
                case renderer::BACKEND::OPENGL:
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                    break;
                case renderer::BACKEND::VULKAN:
                    break;
                default:
                    break;
            }
        }
    }

    void IMGUIManager::ProcessEvent(const SDL_Event& event)
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    void IMGUIManager::RegisterDrawCallback(UIDrawCallback callback)
    {

        m_Callbacks.push_back(std::move(callback));
    }
}