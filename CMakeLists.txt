cmake_minimum_required(VERSION 4.0.2)

project ("HEN SDK" CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL_STATIC ON CACHE BOOL "")
set(SDL_SHARED OFF CACHE BOOL "")
set(SDL_TEST OFF CACHE BOOL "")
set(SDL_INSTALL_TEST OFF CACHE BOOL "")
set(SDL_OPENGL ON CACHE BOOL "")
set(SDL_OPENGLES OFF CACHE BOOL "")
set(SDL_DIRECTX OFF CACHE BOOL "")
set(SDL_RENDER_D3D12 OFF CACHE BOOL "")
set(SDL_RENDER_D3D11 OFF CACHE BOOL "")
set(SDL_RENDER_D3D OFF CACHE BOOL "")
set(SDL_VULKAN OFF CACHE BOOL "")
set(SDL_RENDER_VULKAN OFF CACHE BOOL "")
set(SDL_RENDER_METAL OFF CACHE BOOL "")

FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.2.16
)

FetchContent_MakeAvailable(sdl3)

add_compile_definitions("RELEASE=$<CONFIG:Release>")
add_compile_definitions("DEBUG=$<CONFIG:Debug>")

add_subdirectory("HENEngine")
add_subdirectory("EggEditor")