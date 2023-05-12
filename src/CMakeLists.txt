set(TARGET suggestive-contours)
set(SOURCES
    main.cpp
    Renderer.cpp
    Model.cpp
    Camera.cpp
)

add_executable(${TARGET} ${SOURCES})

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
target_include_directories(imgui PUBLIC "./imgui-docking")
target_include_directories(imgui PUBLIC "./imgui-docking/backends")

target_compile_definitions(${TARGET}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
    GLFW_INCLUDE_NONE
)

target_link_libraries(${TARGET}
    PRIVATE
    glad
    glfw
    imgui
    ${OPENGL_gl_LIBRARY}
)

add_subdirectory(shaders)

