#ifndef DRAW_HPP
#define DRAW_HPP

#include "config.hpp"
#include "engine.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include <vector>
#include <glm/glm.hpp>
using glm::vec2;

class Draw {
public:
    Engine *engine;
    ShaderProgram shader;
    Mesh2D arrowMesh, circleMesh, boxMesh, polylineMesh;
    int maxVerts;
    Draw() {}
    Draw(Engine *engine);
    void mesh(mat4 transform, Mesh2D &mesh, vec3 color, int nElements=-1);
    void circle(mat4 transform, vec2 center, float radius, vec3 color);
    void box(mat4 transform, vec2 center, vec2 size, vec3 color);
    void polyline(mat4 transform, std::vector<vec2> vertices, vec3 color);
    void axes(mat4 transform, float size);
};

inline Draw::Draw(Engine *engine) {
    this->engine = engine;
    shader = ShaderProgram(Config::shaderVert, Config::shaderFrag);
    circleMesh.makeCircle(vec2(0,0), 1);
    circleMesh.createGPUData(engine);
    boxMesh.makeBox(vec2(-0.5,-0.5), vec2(0.5,0.5));
    boxMesh.createGPUData(engine);
    maxVerts = 100;
    polylineMesh.vertices.resize(maxVerts);
    
    for (int i = 0; i < maxVerts-1; i++)
        polylineMesh.edges.push_back(ivec2(i,i+1));
    
    polylineMesh.createGPUData(engine);
    arrowMesh.vertices.push_back(vec2(0,0));
    arrowMesh.vertices.push_back(vec2(1,0));
    arrowMesh.vertices.push_back(vec2(0.7,0.1));
    arrowMesh.vertices.push_back(vec2(0.7,-0.1));
    arrowMesh.edges.push_back(ivec2(0,1));
    arrowMesh.edges.push_back(ivec2(1,2));
    arrowMesh.edges.push_back(ivec2(1,3));
    arrowMesh.createGPUData(engine);
}

inline void Draw::mesh(mat4 transform, Mesh2D &mesh, vec3 color, int nElements) {
    shader.enable();
    shader.setUniform("modelViewMatrix", engine->getMatrix(GL_MODELVIEW)*transform);
    shader.setUniform("projectionMatrix", engine->getMatrix(GL_PROJECTION));
    shader.setUniform("color", color);
    shader.setAttribute("vertex", mesh.vertexBuffer, 2, GL_FLOAT);
    
    if (nElements < 0)
        nElements = mesh.edges.size();
    
    engine->drawElements(GL_LINES, mesh.indexBuffer, nElements*2);
    shader.disable();
}

inline void Draw::circle(mat4 transform, vec2 center, float radius, vec3 color) {
    transform = glm::translate(transform, vec3(center,0));
    transform = glm::scale(transform, radius*vec3(1,1,1));
    mesh(transform, circleMesh, color);
}

inline void Draw::box(mat4 transform, vec2 center, vec2 size, vec3 color) {
    transform = glm::translate(transform, vec3(center, 0));
    transform = glm::scale(transform, vec3(size,0));
    mesh(transform, boxMesh, color);
}

inline void Draw::polyline(mat4 transform, std::vector<vec2> vertices, vec3 color) {
    for (int i = 0; i < vertices.size(); i += maxVerts-1) {
        int start = i, end = i + maxVerts;
	
        if (end > vertices.size())
            end = vertices.size();
	
        engine->copyVertexData(polylineMesh.vertexBuffer,
                               &vertices[start], (end-start)*sizeof(vec2));
        mesh(transform, polylineMesh, color, end-start-1);
    }
}

inline void Draw::axes(mat4 transform, float size) {
    transform = glm::scale(transform, vec3(size,size,size));
    mesh(transform, arrowMesh, vec3(1,0,0));
    transform = glm::rotate(transform, (float)M_PI/2, vec3(0,0,1));
    mesh(transform, arrowMesh, vec3(0,0.8,0));
}

#endif
