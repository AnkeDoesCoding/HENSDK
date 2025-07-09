cmake_minimum_required(VERSION 4.0.2)

project ("HENEngine")

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE THIRDPARTY_FILES CONFIGURE_DEPENDS "thirdparty/*.cpp" "thirdparty/*.c")


add_library(${PROJECT_NAME} STATIC "HENEngine.h" ${SRC_FILES} ${THIRDPARTY_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")
