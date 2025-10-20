set(TARGET suggestive-contours)
set(SOURCES
    main.cpp
    Renderer.cpp
    Model.cpp
    Camera.cpp
)

set(IMGUI_SOURCES
    "./imgui-docking/imgui.cpp"
    "./imgui-docking/imgui_demo.cpp"
    "./imgui-docking/imgui_draw.cpp"
    "./imgui-docking/imgui_tables.cpp"
    "./imgui-docking/imgui_widgets.cpp"
    "./imgui-docking/backends/imgui_impl_glfw.cpp"
    "./imgui-docking/backends/imgui_impl_opengl3.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends"
    "${glfw_SOURCE_DIR}/include" # GLFW include path from FetchContent
)
target_compile_definitions(imgui PUBLIC GLFW_INCLUDE_NONE)

add_executable(${TARGET} ${SOURCES})

# Include directories for the executable
target_include_directories(${TARGET} PRIVATE
    "${CMAKE_SOURCE_DIR}/glad/glad/include"
    "${glm_SOURCE_DIR}"
)


target_compile_definitions(${TARGET} PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(${TARGET}
    PRIVATE
    glad
    glfw
    imgui
    OpenGL::GL
)

if(UNIX AND NOT APPLE)
    if(WAYLAND_FOUND)
        # Link Wayland libraries
        pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
        target_link_libraries(${TARGET} PRIVATE ${WAYLAND_LIBRARIES})
    elseif(X11_FOUND)
        # Link X11 and related libraries
        target_link_libraries(${TARGET} PRIVATE X11::X11 X11::Xrandr X11::Xinerama X11::Xcursor X11::Xi)
    endif()
elseif(APPLE)
    # macOS: Link Cocoa, IOKit, CoreVideo
    target_link_libraries(${TARGET} PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(WIN32)
    # Windows: Link gdi32
    target_link_libraries(${TARGET} PRIVATE gdi32)
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
