cmake_minimum_required(VERSION 3.11...4.1)
project(suggestive-contours)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND QUIET wayland-client)
    pkg_check_modules(X11 QUIET x11)

    if(WAYLAND_FOUND)
        message(STATUS "Wayland found; enabling Wayland support")
        set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
        set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
    elseif(X11_FOUND)
        message(STATUS "Wayland not found; falling back to X11")
        set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    else()
        message(FATAL_ERROR "Neither Wayland nor X11 found; install libx11-dev or wayland-devel")
    endif()
else()
    # Non-Linux: Disable both Wayland and X11
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

# include_directories(glad)
# include_directories(include ${GLM_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

add_subdirectory(glad)
add_subdirectory(src)

file(COPY "${CMAKE_SOURCE_DIR}/config" DESTINATION "${CMAKE_BINARY_DIR}/src")