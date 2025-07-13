cmake_minimum_required(VERSION 4.0.2)

project ("HEN SDK" CXX)

#make sure to delete the build folder and reconfigure after changing this 
set(SHIPPING_BUILD OFF CACHE BOOL "Turn this on for shipping" FORCE)

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

if(${SHIPPING_BUILD})
    message("-- [HEN SDK] Shipping Build: ON")

    target_compile_definitions("HENEngine" PUBLIC ENGINE_RESOURCES_PATH="./res/engine/")
    target_compile_definitions("EggEditor" PUBLIC ENGINE_RESOURCES_PATH="./res/engine/")

else()
    message("-- [HEN SDK] Shipping Build: OFF")

    target_compile_definitions("HENEngine" PUBLIC ENGINE_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/HENEngine/res/")
    target_compile_definitions("EggEditor" PUBLIC ENGINE_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/HENEngine/res/")

endif()
