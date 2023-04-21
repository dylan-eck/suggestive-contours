cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_COMPILER "C:/Program Files/w64devkit/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files/w64devkit/bin/g++.exe")

set(glm_DIR "C:/Program Files/GLM/lib/cmake/glm")
set(glfw3_DIR "C:/Program Files/GLFW/lib/cmake/glfw3")

project(dylanEck_A2)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(glad)

add_subdirectory(glad)
add_subdirectory(src)

