#ifndef _HENUI_H_
#define _HENUI_H_

#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <functional>
#include <vector>

namespace hen::ui
{
    struct DrawCallback
    {
        std::function<void()> Callback;
        bool Important = false;
    };

    class IMGUIManager
    {
    public:
        void Initialise(SDL_Window* window);
        void Shutdown();

        void Render();

        void ProcessEvent(const SDL_Event& event);

        void RegisterDrawCallback(std::function<void()> callback, bool isImportant = false);

        uint32_t GetDockSpaceID();
    
    public: 
        bool Initialised = false;

    private:
        static inline std::vector<DrawCallback> m_Callbacks;
    };

    inline IMGUIManager*& GetIMGUIManager()
	{
		static IMGUIManager* manager = nullptr;
		return manager;
	}
}

#endif //!_HENUI_H_