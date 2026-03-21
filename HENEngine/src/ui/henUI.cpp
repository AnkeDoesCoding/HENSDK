#include "ui/henUI.h"

#include "renderer/henRenderer.h"
#include "core/henCVar.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace hen::ui
{
    cvar::CVar cvar_ScalingFactor("ui_scaling_factor", 1.0f, cvar::FLAGS_ARCHIVE, []()
    {
        ImGui::GetIO().FontGlobalScale = ImGui::GetIO().FontGlobalScale * cvar_ScalingFactor.GetFloat();
    });

    void IMGUIManager::Initialise(SDL_Window* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_FrameBg] = ImVec4(style.Colors[ImGuiCol_FrameBg].x, style.Colors[ImGuiCol_FrameBg].y, style.Colors[ImGuiCol_FrameBg].z, 1.0f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(style.Colors[ImGuiCol_WindowBg].x, style.Colors[ImGuiCol_WindowBg].y, style.Colors[ImGuiCol_WindowBg].z, 1.0f);

        switch (renderer::CurrentBackend)
        {
            case renderer::BACKENDS::OPENGL:
                ImGui_ImplSDL3_InitForOpenGL(window, SDL_GL_GetCurrentContext());
                ImGui_ImplOpenGL3_Init("#version 460");
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
            case renderer::BACKENDS::OPENGL:
                ImGui_ImplOpenGL3_Shutdown();
                break;
            case renderer::BACKENDS::VULKAN:
                break;
            default:
                break;
        }

        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

    }

    void IMGUIManager::Render()
    {
        if (!Initialised)
        {
            return;
        }

        ImGui_ImplSDL3_NewFrame();
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKENDS::OPENGL:
                ImGui_ImplOpenGL3_NewFrame();
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

        for (ui::UIDrawCallback& callback : m_Callbacks)
        {
            callback();
        }

        ImGui::Render();

        switch (renderer::CurrentBackend)
        {
            case renderer::BACKENDS::OPENGL:
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                break;
            default:
                break;
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