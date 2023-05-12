#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <tuple>
#include <algorithm>
#include <iomanip>

struct Vec3 {
    float x;
    float y;
    float z;

    Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    Vec3 operator/(float scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }
};

float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3 normalize(const Vec3& v) {
    float length = std::sqrt(dot(v, v));
    return v / length;
}

void calculateNormals(
    const std::vector<Vec3>& vertices,
    const std::vector<int>& indices,
    std::vector<Vec3>& normals
) {
    normals.resize(vertices.size(), {0, 0, 0});

    for (size_t i = 0; i < indices.size(); i += 3) {
        Vec3 v0 = vertices[indices[i] - 1];
        Vec3 v1 = vertices[indices[i + 1] - 1];
        Vec3 v2 = vertices[indices[i + 2] - 1];

        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;
        Vec3 faceNormal = cross(edge1, edge2);

        normals[indices[i] - 1] = normals[indices[i] - 1] + faceNormal;
        normals[indices[i + 1] - 1] = normals[indices[i + 1] - 1] + faceNormal;
        normals[indices[i + 2] - 1] = normals[indices[i + 2] - 1] + faceNormal;
    }

    for (size_t i = 0; i < normals.size(); i++) {
        normals[i] = normalize(normals[i]);
    }
}

void parseObjFile(
    const std::string& filename,
    std::vector<Vec3>& vertices,
    std::vector<int>& indices,
    std::unordered_map<int, std::unordered_set<int> >& adjacency_list
) {
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(objFile, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (prefix == "f") {
            int face[3];
            char slash;
            for (int i = 0; i < 3; ++i) {
                ss >> face[i] >> slash;
                ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
            }

            indices.push_back(face[0]);
            indices.push_back(face[1]);
            indices.push_back(face[2]);

            if (adjacency_list.find(face[0]) == adjacency_list.end()) {
                adjacency_list.insert({face[0], {face[1], face[2]}});
            } else {
                adjacency_list[face[0]].insert(face[1]);
                adjacency_list[face[0]].insert(face[2]);
            }

            if (adjacency_list.find(face[1]) == adjacency_list.end()) {
                adjacency_list.insert({face[1], {face[0], face[2]}});
            } else {
                adjacency_list[face[1]].insert(face[0]);
                adjacency_list[face[1]].insert(face[2]);
            }

            if (adjacency_list.find(face[2]) == adjacency_list.end()) {
                adjacency_list.insert({face[2], {face[0], face[1]}});
            } else {
                adjacency_list[face[2]].insert(face[0]);
                adjacency_list[face[2]].insert(face[1]);
            }
        }
    }

    objFile.close();
}

void writeObjFile(
    const std::string& filename,
    const std::vector<Vec3>& vertices,
    const std::vector<int>& indices,
    const std::vector<Vec3>& normals
) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error opening output file: " << filename << std::endl;
        return;
    }

    outFile << std::fixed << std::setprecision(6);

    // Write vertices
    for (const Vec3& vertex : vertices) {
        outFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
    }

    // Write normals
    for (const Vec3& normal : normals) {
        outFile << "vn " << normal.x << " " << normal.y << " " << normal.z << std::endl;
    }

    // Write faces
    for (size_t i = 0; i < indices.size(); i += 3) {
        outFile << "f "
                << indices[i] << "//" << indices[i] << " "
                << indices[i + 1] << "//" << indices[i + 1] << " "
                << indices[i + 2] << "//" << indices[i + 2]
                << std::endl;
    }

    outFile.close();
}

int main() {
    std::vector<Vec3> vertices;
    std::vector<int> indices;
    std::vector<Vec3> normals;
    std::unordered_map<int, std::unordered_set<int> > adjacency_list;

    parseObjFile("./voxel_blob.obj", vertices, indices, adjacency_list);

    std::vector<Vec3> newVertices(vertices.size());

    int interation_count = 90;
    float lambda = 0.33;
    float mu = -0.34;

    for (int k = 0; k < interation_count; k++) {
        for (int i = 0; i < vertices.size(); i++) {
            Vec3 deltaVi;
            deltaVi.x = 0.0;
            deltaVi.y = 0.0;
            deltaVi.z = 0.0;

            float Wij = 1.0 / adjacency_list[i + 1].size();

            for (const auto& index : adjacency_list[i + 1]) {

                deltaVi = deltaVi + (vertices[index - 1] - vertices[i]) * Wij;
            }

            Vec3 newPositon = vertices[i] + deltaVi * lambda;

            newVertices[i] = newPositon;
        }

        vertices = newVertices;

        for (int i = 0; i < vertices.size(); i++) {
            Vec3 deltaVi;
            deltaVi.x = 0.0;
            deltaVi.y = 0.0;
            deltaVi.z = 0.0;

            float Wij = 1.0 / adjacency_list[i + 1].size();

            for (const auto& index : adjacency_list[i + 1]) {

                deltaVi = deltaVi + (vertices[index - 1] - vertices[i]) * Wij;
            }

            Vec3 newPositon = vertices[i] + deltaVi * mu;

            newVertices[i] = newPositon;
        }

        vertices = newVertices;
    }

    calculateNormals(vertices, indices, normals);
    writeObjFile("./out.obj", vertices, indices, normals);

    return 0;
}