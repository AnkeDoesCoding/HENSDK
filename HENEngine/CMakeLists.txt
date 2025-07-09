cmake_minimum_required(VERSION 4.0.2)

project ("HENEngine" CXX)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_library(${PROJECT_NAME} STATIC "HENEngine.h" ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")