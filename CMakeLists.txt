cmake_minimum_required(VERSION 4.0.2)

project ("HEN SDK" CXX)

# Make sure to delete the build folder and reconfigure after changing this 
set(SHIPPING_BUILD OFF CACHE BOOL "" FORCE)

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
    GIT_TAG release-3.4.0
)

FetchContent_MakeAvailable(sdl3)

add_compile_definitions("RELEASE=$<CONFIG:Release>")
add_compile_definitions("DEBUG=$<CONFIG:Debug>")

if(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    add_compile_definitions(PLATFORM_LINUX)
endif()


if(${SHIPPING_BUILD})

    add_compile_definitions(ENGINE_RESOURCE_PATH="./res/engine/")

else()

    add_compile_definitions(ENGINE_RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/HENEngine/res/")

endif()

add_subdirectory("HENEngine")
add_subdirectory("EggEditor")

