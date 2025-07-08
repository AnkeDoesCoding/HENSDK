cmake_minimum_required(VERSION 4.0.2)

project ("HENEngine" CXX)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_library(${PROJECT_NAME} STATIC "HENEngine.h" ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")