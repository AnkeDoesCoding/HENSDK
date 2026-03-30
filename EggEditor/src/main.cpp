
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

    // TODO: ENGINE HAS TO HANDLE THIS SOMEHOW

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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