cmake_minimum_required(VERSION 4.0.2)

project ("HEN SDK" CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions("RELEASE=$<CONFIG:Release>")
add_compile_definitions("DEBUG=$<CONFIG:Debug>")

add_subdirectory("HENEngine")
add_subdirectory("EggEditor")