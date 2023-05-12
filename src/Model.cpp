#include "Model.hpp"
#include "tiny_obj_loader.h"
#include <unordered_map>
#include <stdexcept>
#include <iostream>

// this function loads a model from a .obj file
Model Model::fromOBJ(const char* filePath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath)) {
        throw std::runtime_error(warn + err);
    }

    Model model;
    std::unordered_map<int, uint32_t> vertexIndicesMap;
    uint32_t nextIndex = 0;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            int vertexIndex = index.vertex_index;
            int normalIndex = index.normal_index;

            int combinedIndex = vertexIndex * attrib.normals.size() + normalIndex;

            auto itr = vertexIndicesMap.find(combinedIndex);
            if (itr == vertexIndicesMap.end()) {
                model.vertexData.push_back(attrib.vertices[3 * vertexIndex + 0]);
                model.vertexData.push_back(attrib.vertices[3 * vertexIndex + 1]);
                model.vertexData.push_back(attrib.vertices[3 * vertexIndex + 2]);

                model.colorData.push_back(1.0f);
                model.colorData.push_back(1.0f);
                model.colorData.push_back(1.0f);

                model.normalData.push_back(attrib.normals[3 * normalIndex + 0]);
                model.normalData.push_back(attrib.normals[3 * normalIndex + 1]);
                model.normalData.push_back(attrib.normals[3 * normalIndex + 2]);

                vertexIndicesMap[combinedIndex] = nextIndex;
                model.indexData.push_back(nextIndex);
                ++nextIndex;
            } else {
                model.indexData.push_back(itr->second);
            }
        }
    }

    return model;
}

void Model::initGPUresources() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vBufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, vBufHandle);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &iBufHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBufHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(uint32_t), indexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &cBufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, cBufHandle);
    glBufferData(GL_ARRAY_BUFFER, colorData.size() * sizeof(float), colorData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &nBufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, nBufHandle);
    glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(float), normalData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vBufHandle);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    glBindBuffer(GL_ARRAY_BUFFER, cBufHandle);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

    glBindBuffer(GL_ARRAY_BUFFER, nBufHandle);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    glBindVertexArray(0);
}