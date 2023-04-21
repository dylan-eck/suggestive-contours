#ifndef RENDERER_HPP
#define RENDERER_HPP

// class for handling rendering functionality

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Model.hpp"

class Renderer {
public:
    void setShaderProgram(GLuint programHandle);
    void drawModel(Model model);

private:
    GLuint shaderProgram;
};

#endif