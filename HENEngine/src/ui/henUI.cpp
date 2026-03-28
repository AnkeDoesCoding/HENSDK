#include "ui/henUI.h"

#include "renderer/henRenderer.h"
#include "core/henCVar.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace hen::ui
{
    static uint32_t DockSpaceID = 0;

    cvar::CVar cvar_ScalingFactor("ui_scaling_factor", 1.0f, cvar::FLAGS_ARCHIVE, []()
    {
        ImGui::GetIO().FontGlobalScale = ImGui::GetIO().FontGlobalScale * cvar_ScalingFactor.GetFloat();
    });

    cvar::CVar cvar_Visible("ui_visible", true, cvar::FLAGS_NONE);

    void IMGUIManager::Initialise(SDL_Window* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_FrameBg] = ImVec4(style.Colors[ImGuiCol_FrameBg].x, style.Colors[ImGuiCol_FrameBg].y, style.Colors[ImGuiCol_FrameBg].z, 1.0f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(style.Colors[ImGuiCol_WindowBg].x, style.Colors[ImGuiCol_WindowBg].y, style.Colors[ImGuiCol_WindowBg].z, 1.0f);

        style.Alpha = 1.0;

        style.WindowPadding = ImVec2(10, 10);
        style.FramePadding = ImVec2(4, 4);
        style.CellPadding = ImVec2(4, 2);
        style.ItemSpacing = ImVec2(8, 6);
        style.ItemInnerSpacing = ImVec2(6, 4);
        style.IndentSpacing = 20.0f;
        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 10.0f;

        const float rounding = 0.0f;
        style.WindowRounding = rounding;
        style.ChildRounding = rounding;
        style.FrameRounding = rounding;
        style.PopupRounding = rounding;
        style.ScrollbarRounding = rounding;
        style.GrabRounding = rounding;
        style.TabRounding = rounding;

        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;

        style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.98f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.58f, 0.12f, 0.20f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.95f, 0.58f, 0.12f, 0.40f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.95f, 0.58f, 0.12f, 0.60f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.95f, 0.58f, 0.12f, 0.80f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.68f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.68f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.68f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.95f, 0.58f, 0.12f, 0.80f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.95f, 0.58f, 0.12f, 0.90f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.68f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.95f, 0.58f, 0.12f, 0.30f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.95f, 0.58f, 0.12f, 0.70f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.95f, 0.58f, 0.12f, 0.30f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.95f, 0.58f, 0.12f, 0.70f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.60f, 0.36f, 0.08f, 1.00f);
        style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.95f, 0.58f, 0.12f, 0.50f);
        style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.68f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.95f, 0.58f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.68f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.95f, 0.58f, 0.12f, 0.30f);
        style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.95f, 0.58f, 0.12f, 0.15f);
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.95f, 0.58f, 0.12f, 0.05f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.95f, 0.58f, 0.12f, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.95f, 0.58f, 0.12f, 0.90f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.55f);
        

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
        ImGui::Begin("InvisibleDockspace", nullptr, windowFlags);
        ImGui::PopStyleVar(3);

        DockSpaceID = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(DockSpaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton);
        ImGui::End();

        for (ui::DrawCallback& drawCall : m_Callbacks)
        {
            if (!cvar_Visible.GetBool() && !drawCall.Important)
            {
                continue;
            }

            drawCall.Callback();
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

    void IMGUIManager::RegisterDrawCallback(std::function<void()> callback, bool isImportant)
    {

        m_Callbacks.push_back({std::move(callback), isImportant });
    }

    uint32_t IMGUIManager::GetDockSpaceID()
    {
        return DockSpaceID;
    }
}