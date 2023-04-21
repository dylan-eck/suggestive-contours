#include "Renderer.hpp"

// set the opengl program to be used for future drawModel() calls
void Renderer::setShaderProgram(GLuint programHandle) {
    shaderProgram = programHandle;
    glUseProgram(shaderProgram);
}

// draw a model object using the previously set shader program
void Renderer::drawModel(Model model) {
    GLuint modelMatrixUniformLocation = glGetUniformLocation(shaderProgram, "mMat");
    glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &model.modelMatrix[0][0]);
    glBindVertexArray(model.vao);
    glDrawElements(GL_TRIANGLES, model.indexData.size(), GL_UNSIGNED_INT, nullptr);
}