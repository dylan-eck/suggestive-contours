#include "Model.hpp"
#include "tiny_obj_loader.h"
#include <stdexcept>

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
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            model.vertexData.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            model.vertexData.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            model.vertexData.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            model.colorData.push_back(1.0f);
            model.colorData.push_back(1.0f);
            model.colorData.push_back(1.0f);

            model.normalData.push_back(attrib.normals[3 * index.normal_index + 0]);
            model.normalData.push_back(attrib.normals[3 * index.normal_index + 1]);
            model.normalData.push_back(attrib.normals[3 * index.normal_index + 2]);

            model.indexData.push_back(model.indexData.size());
        }
    }

    glGenVertexArrays(1, &model.vao);
    glBindVertexArray(model.vao);

    glGenBuffers(1, &model.vBufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, model.vBufHandle);
    glBufferData(GL_ARRAY_BUFFER, model.vertexData.size() * sizeof(float), model.vertexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &model.iBufHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.iBufHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indexData.size() * sizeof(uint32_t), model.indexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &model.cBufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, model.cBufHandle);
    glBufferData(GL_ARRAY_BUFFER, model.colorData.size() * sizeof(float), model.colorData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &model.nBufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, model.nBufHandle);
    glBufferData(GL_ARRAY_BUFFER, model.normalData.size() * sizeof(float), model.normalData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, model.vBufHandle);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    glBindBuffer(GL_ARRAY_BUFFER, model.cBufHandle);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

    glBindBuffer(GL_ARRAY_BUFFER, model.nBufHandle);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    glBindVertexArray(0);

    return model;
}