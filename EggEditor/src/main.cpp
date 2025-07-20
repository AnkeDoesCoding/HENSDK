
#define SDL_MAIN_USE_CALLBACKS 1
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "HENEngine.h"

SDL_Window* Window;
hen::Application EggEditor;

SDL_AppResult SDL_AppInit(void** appState, int argC, char* argV[])
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        return SDL_APP_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

    Window = SDL_CreateWindow("Egg Editor", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    EggEditor.SetWindow(Window);

    EggEditor.Initialise();

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
        // resize shit goes here
        EggEditor.SetWindow(Window);
    }

    if(hen::input::Press(hen::input::BUTTON::KEYBOARD_BUTTON_ESCAPE))
    {
        EggEditor.Shutdown();
        SDL_Quit();
        return SDL_APP_SUCCESS; 
    }

    hen::input::ProcessEvent(*event);
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