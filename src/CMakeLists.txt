set(TARGET suggestive-contours)
set(SOURCES main.cpp Renderer.cpp Model.cpp Camera.cpp)

add_executable(${TARGET} ${SOURCES})

target_compile_definitions(${TARGET}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
    GLFW_INCLUDE_NONE
)

target_link_libraries(${TARGET}
    PRIVATE
    glad
    glfw
    ${OPENGL_gl_LIBRARY}
)

add_subdirectory(shaders)