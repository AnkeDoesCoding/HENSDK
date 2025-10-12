
#define SDL_MAIN_USE_CALLBACKS 1
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "EggEditor.h"

SDL_Window* Window;
Editor EggEditor;

SDL_AppResult SDL_AppInit(void** appState, int argC, char* argV[])
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        return SDL_APP_FAILURE;
    }

    Window = SDL_CreateWindow("Egg Editor", 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    hen::arguments::Parse(argC, argV);

    EggEditor.Initialise(Window);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event) 
{
    if (event->type == SDL_EVENT_QUIT) 
    {
        return SDL_APP_SUCCESS; 
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED) 
    {
        EggEditor.ResizeWindow();
    }

    if(hen::input::Press(hen::input::BUTTON::KEYBOARD_BUTTON_ESCAPE))
    {
        return SDL_APP_SUCCESS; 
    }

    EggEditor.ProcessEvent(*event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState) 
{
    EggEditor.Run();

    SDL_UpdateWindowSurface(Window);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appState, SDL_AppResult result)
{
    EggEditor.Shutdown();

    SDL_Quit();
}