cmake_minimum_required(VERSION 4.0.2)

project ("EggEditor" CXX)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp
    GIT_TAG v6.0.2
)

FetchContent_MakeAvailable(assimp)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE HENEngine assimp)

