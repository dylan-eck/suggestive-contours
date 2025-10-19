cmake_minimum_required(VERSION 3.11)
project(suggestive-contours)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(glad)
include_directories(include ${GLM_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

add_subdirectory(glad)
add_subdirectory(src)