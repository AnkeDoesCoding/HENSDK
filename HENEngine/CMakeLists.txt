cmake_minimum_required(VERSION 4.0.2)

project ("HENEngine")

add_subdirectory(vendor/JoltPhysics/Build)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE IMGUI_FILES CONFIGURE_DEPENDS "vendor/imgui/*.cpp")

add_library(${PROJECT_NAME} STATIC "HENEngine.h" ${SRC_FILES} ${IMGUI_FILES} "vendor/glad/src/glad.c" "vendor/stb/src/stb_image.cpp" )

target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 Jolt)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")
