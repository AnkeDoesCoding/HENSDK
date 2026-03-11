#ifndef _HENUI_H_
#define _HENUI_H_

#include <SDL3/SDL.h>
#include <imgui/imgui.h>

#include <functional>
#include <vector>

namespace hen::ui
{
    using UIDrawCallback = std::function<void()>;

    class IMGUIManager
    {
    public:
        void Initialise(SDL_Window* window);
        void Shutdown();

        void BeginFrame();
        void EndFrame();

        void ProcessEvent(const SDL_Event& event);

        void RegisterDrawCallback(UIDrawCallback callback);
    
    public: 
        bool Initialised = false;

    private:
        static inline std::vector<UIDrawCallback> m_Callbacks;
    };

    inline IMGUIManager*& GetIMGUIManager()
	{
		static IMGUIManager* manager = nullptr;
		return manager;
	}
}

#endif //!_HENUI_H_