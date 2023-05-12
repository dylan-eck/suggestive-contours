#ifndef MODEL_HPP
#define MODEL_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

class Model {
public:
    static Model fromOBJ(const char* filePath);

    void initGPUresources();

    std::vector<float> vertexData;
    std::vector<float> colorData;
    std::vector<float> normalData;
    std::vector<uint32_t> indexData;

    GLuint vao;
    GLuint vBufHandle;
    GLuint iBufHandle;
    GLuint cBufHandle;
    GLuint nBufHandle;

    glm::mat4 modelMatrix{glm::mat4(1.0f)};
};

#endif