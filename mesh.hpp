#ifndef MESH_HPP
#define MESH_HPP

#include "engine.hpp"
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
using glm::vec2;
using glm::vec3;
using glm::ivec2;
using glm::ivec3;

class Mesh2D {
public:
    void makeCircle(vec2 center, float radius);
    void makeBox(vec2 xymin, vec2 xymax);
    void createGPUData(Engine *engine);
    std::vector<vec2> vertices;
    std::vector<ivec2> edges;
    VertexBuffer vertexBuffer;
    ElementBuffer indexBuffer;
};

inline void Mesh2D::makeCircle(vec2 center, float radius) {
    int n = 30;
    for (int i = 0; i < 30; i++) {
        float t = 2*M_PI*i/n;
        vertices.push_back(center + radius*vec2(cos(t),sin(t)));
        edges.push_back(ivec2(i,(i+1)%n));
    }
    vertices.push_back(center + radius*vec2(-0.5,0));
    vertices.push_back(center + radius*vec2(+0.5,0));
    vertices.push_back(center + radius*vec2(0,-0.5));
    vertices.push_back(center + radius*vec2(0,+0.5));
    edges.push_back(ivec2(n+0,n+1));
    edges.push_back(ivec2(n+2,n+3));
}

inline void Mesh2D::makeBox(vec2 xymin, vec2 xymax) {
    vertices.push_back(vec2(xymin.x, xymin.y));
    vertices.push_back(vec2(xymax.x, xymin.y));
    vertices.push_back(vec2(xymax.x, xymax.y));
    vertices.push_back(vec2(xymin.x, xymax.y));
    vec2 center = (xymin + xymax)/2.f;
    vertices.push_back(vec2((xymin.x+center.x)/2, center.y));
    vertices.push_back(vec2((xymax.x+center.x)/2, center.y));
    vertices.push_back(vec2(center.x, (xymin.y+center.y)/2));
    vertices.push_back(vec2(center.x, (xymax.y+center.y)/2));
    edges.push_back(ivec2(0,1));
    edges.push_back(ivec2(1,2));
    edges.push_back(ivec2(2,3));
    edges.push_back(ivec2(3,0));
    edges.push_back(ivec2(4,5));
    edges.push_back(ivec2(6,7));
}

inline void Mesh2D::createGPUData(Engine *engine) {
    vertexBuffer = engine->allocateVertexBuffer(vertices);
    std::vector<int> indices;
    for (int e = 0; e < edges.size(); e++) {
        indices.push_back(edges[e][0]);
        indices.push_back(edges[e][1]);
    }
    indexBuffer = engine->allocateElementBuffer(indices);
}

class Mesh {
public:
    void loadOBJ(const std::string &filename);
    void makeRectXY(vec2 xymin, vec2 xymax, float z=0);
    void makeRectYZ(vec2 yzmin, vec2 yzmax, float x=0);
    void makeRectXZ(vec2 xzmin, vec2 xzmax, float y=0);
    void makeSphere(vec3 center, float radius, int slices, int stacks);
    void makeBox(vec3 xyzmin, vec3 xyzmax);
    void createGPUData(Engine *engine);
    std::vector<vec3> vertices;   // vertex positions
    std::vector<vec3> colors;     // vertex colors
    std::vector<vec3> normals;    // vertex normals
    std::vector<vec2> texCoords;  // vertex texture coordinates
    std::vector<ivec3> triangles; // triangle vertex indices
    VertexBuffer vertexBuffer, colorBuffer, normalBuffer, texCoordBuffer;
    ElementBuffer indexBuffer;
};

inline void Mesh::makeRectXY(vec2 xymin, vec2 xymax, float z) {
    vertices.push_back(vec3(xymin[0], xymin[1], z));
    vertices.push_back(vec3(xymax[0], xymin[1], z));
    vertices.push_back(vec3(xymax[0], xymax[1], z));
    vertices.push_back(vec3(xymin[0], xymax[1], z));
    normals.push_back(vec3(0,0,1));
    normals.push_back(vec3(0,0,1));
    normals.push_back(vec3(0,0,1));
    normals.push_back(vec3(0,0,1));
    texCoords.push_back(vec2(0,0));
    texCoords.push_back(vec2(1,0));
    texCoords.push_back(vec2(1,1));
    texCoords.push_back(vec2(0,1));
    triangles.push_back(ivec3(0,1,2));
    triangles.push_back(ivec3(0,2,3));
}

inline void Mesh::makeRectYZ(vec2 yzmin, vec2 yzmax, float x) {
    vertices.push_back(vec3(x, yzmin[0], yzmin[1]));
    vertices.push_back(vec3(x, yzmax[0], yzmin[1]));
    vertices.push_back(vec3(x, yzmax[0], yzmax[1]));
    vertices.push_back(vec3(x, yzmin[0], yzmax[1]));
    normals.push_back(vec3(1,0,0));
    normals.push_back(vec3(1,0,0));
    normals.push_back(vec3(1,0,0));
    normals.push_back(vec3(1,0,0));
    texCoords.push_back(vec2(1,0));
    texCoords.push_back(vec2(1,1));
    texCoords.push_back(vec2(0,1));
    texCoords.push_back(vec2(0,0));
    triangles.push_back(ivec3(0,1,2));
    triangles.push_back(ivec3(0,2,3));
}

inline void Mesh::makeRectXZ(vec2 xzmin, vec2 xzmax, float y) {
    vertices.push_back(vec3(xzmin[0], y, xzmin[1]));
    vertices.push_back(vec3(xzmin[0], y, xzmax[1]));
    vertices.push_back(vec3(xzmax[0], y, xzmax[1]));
    vertices.push_back(vec3(xzmax[0], y, xzmin[1]));
    normals.push_back(vec3(0,1,0));
    normals.push_back(vec3(0,1,0));
    normals.push_back(vec3(0,1,0));
    normals.push_back(vec3(0,1,0));
    texCoords.push_back(vec2(0,1));
    texCoords.push_back(vec2(0,0));
    texCoords.push_back(vec2(1,0));
    texCoords.push_back(vec2(1,1));
    triangles.push_back(ivec3(0,1,2));
    triangles.push_back(ivec3(0,2,3));
}

inline void Mesh::makeSphere(vec3 center, float radius, int slices, int stacks) {
    for (int j = 0; j <= stacks; j++) {
        for (int i = 0; i <= slices; i++) {
            float lat = M_PI*((float)j/stacks - 0.5);
            float lon = 2*M_PI*((float)i/slices - 0.5);
            vec3 v = vec3(sin(lon)*cos(lat), sin(lat), cos(lon)*cos(lat));
            vertices.push_back(center + radius*v);
            normals.push_back(v);
            texCoords.push_back(vec2((float)i/slices, (float)j/stacks));
        }
    }
    for (int j = 0; j < stacks; j++) {
        for (int i = 0; i < slices; i++) {
            triangles.push_back(ivec3(
                (i+0)+(j+0)*(slices+1),
                (i+1)+(j+0)*(slices+1),
                (i+0)+(j+1)*(slices+1)));
            triangles.push_back(ivec3(
                (i+1)+(j+0)*(slices+1),
                (i+1)+(j+1)*(slices+1),
                (i+0)+(j+1)*(slices+1)));
        }
    }
}

inline void Mesh::makeBox(vec3 xyzmin, vec3 xyzmax) {
    vertices.push_back(vec3(xyzmin[0], xyzmin[1], xyzmin[2]));
    vertices.push_back(vec3(xyzmax[0], xyzmin[1], xyzmin[2]));
    vertices.push_back(vec3(xyzmin[0], xyzmax[1], xyzmin[2]));
    vertices.push_back(vec3(xyzmax[0], xyzmax[1], xyzmin[2]));
    vertices.push_back(vec3(xyzmin[0], xyzmin[1], xyzmax[2]));
    vertices.push_back(vec3(xyzmax[0], xyzmin[1], xyzmax[2]));
    vertices.push_back(vec3(xyzmin[0], xyzmax[1], xyzmax[2]));
    vertices.push_back(vec3(xyzmax[0], xyzmax[1], xyzmax[2]));
    triangles.push_back(ivec3(0,2,3));
    triangles.push_back(ivec3(0,3,1));
    triangles.push_back(ivec3(0,4,6));
    triangles.push_back(ivec3(0,6,2));
    triangles.push_back(ivec3(0,1,5));
    triangles.push_back(ivec3(0,5,4));
    triangles.push_back(ivec3(4,5,7));
    triangles.push_back(ivec3(4,7,6));
    triangles.push_back(ivec3(1,3,7));
    triangles.push_back(ivec3(1,7,5));
    triangles.push_back(ivec3(2,6,7));
    triangles.push_back(ivec3(2,7,3));
}

inline void Mesh::createGPUData(Engine *engine) {
    vertexBuffer = engine->allocateVertexBuffer(vertices);
    if (!colors.empty())
        colorBuffer = engine->allocateVertexBuffer(colors);
    if (!normals.empty())
        normalBuffer = engine->allocateVertexBuffer(normals);
    if (!texCoords.empty())
        texCoordBuffer = engine->allocateVertexBuffer(texCoords);
    std::vector<int> indices;
    for (int t = 0; t < triangles.size(); t++) {
        indices.push_back(triangles[t][0]);
        indices.push_back(triangles[t][1]);
        indices.push_back(triangles[t][2]);
    }
    indexBuffer = engine->allocateElementBuffer(indices);
}

// Extremely crude OBJ file loader. May not work for general OBJ files.
// Do not use outside of the simple example meshes provided in this course.
inline void Mesh::loadOBJ(const std::string &filename) {
    std::fstream file(filename.c_str(), std::ios::in);
    if (!file) {
        Engine::errorMessage("Failed to load " + filename);
        exit(EXIT_FAILURE);
    }
    while (file) {
        std::string line;
        do
            getline(file, line);
        while (file && (line.length() == 0 || line[0] == '#'));
        std::stringstream linestream(line);
        std::string keyword;
        linestream >> keyword;
        if (keyword == "v") {
            vec3 vertex;
            linestream >> vertex[0] >> vertex[1] >> vertex[2];
            vertices.push_back(vertex);
        } else if (keyword == "vn") {
            vec3 normal;
            linestream >> normal[0] >> normal[1] >> normal[2];
            normals.push_back(normal);
        } else if (keyword == "vt") {
            vec2 texCoord;
            linestream >> texCoord[0] >> texCoord[1];
            texCoords.push_back(texCoord);
        } else if (keyword == "f") {
            std::vector<int> polygon;
            std::string word;
            while (linestream >> word) {
                std::stringstream wstream(word);
                int v;
                wstream >> v;
                polygon.push_back(v-1); // In OBJ files, indices start from 1
            }
            for (int i = 2; i < polygon.size(); i++)
                triangles.push_back(ivec3(polygon[0], polygon[i-1], polygon[i]));
        }
    }
}

#endif
