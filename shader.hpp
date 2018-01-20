#ifndef SHADER_HPP
#define SHADER_HPP

#include "engine.hpp"
#include <fstream>
#include <glm/glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class ShaderProgram {
public:
    ShaderProgram(): vertexShader(0), fragmentShader(0), program(0), vao(0) {}
    ShaderProgram(std::string vertFile, std::string fragFile);
    void setAttribute(std::string name, VertexBuffer buffer, int dim, GLenum type);
    void setUniform(std::string name, int i);
    void setUniform(std::string name, float f);
    void setUniform(std::string name, vec2 v);
    void setUniform(std::string name, vec3 v);
    void setUniform(std::string name, vec4 v);
    void setUniform(std::string name, mat4 m);
    void setTexture(std::string name, Texture tex, int texUnit);
    void enable();
    void disable();
protected:
    GLuint vertexShader, fragmentShader;
    GLuint program;
    GLuint vao;
    GLuint loadShader(GLenum type, std::string filename);
};

inline ShaderProgram::ShaderProgram(std::string vertFile, std::string fragFile) {
    vertexShader = loadShader(GL_VERTEX_SHADER, vertFile);
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragFile);
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char infolog[512];
        glGetProgramInfoLog(program, 512, NULL, infolog);
        Engine::errorMessage(std::string("Linking of shader program failed:\n") + infolog);
        exit(EXIT_FAILURE);
    }
    glGenVertexArrays(1, &vao);
    Engine::dieIfOpenGLError();
}

inline GLuint ShaderProgram::loadShader(GLenum type, std::string filename) {
    std::fstream file(filename, std::ios::in);
    if (!file) {
        Engine::errorMessage("Failed to load file " + filename);
        exit(EXIT_FAILURE);
    }
    std::stringstream sstr;
    sstr << file.rdbuf();
    std::string str = sstr.str();
    const char* source = str.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char infolog[512];
        glGetShaderInfoLog(shader, 512, NULL, infolog);
        Engine::errorMessage("Compilation of shader " + filename + " failed:\n" + infolog);
        exit(EXIT_FAILURE);
    }
    Engine::dieIfOpenGLError();
    return shader;
}

inline void ShaderProgram::setAttribute(std::string name, VertexBuffer buffer, int dim, GLenum type) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GLint attrib = glGetAttribLocation(program, name.c_str());
    if (attrib != -1) {
        glVertexAttribPointer(attrib, dim, type, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(attrib);
    }
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::setUniform(std::string name, int i) {
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniform1i(uniform, i);
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::setUniform(std::string name, float f) {
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniform1f(uniform, f);
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::setUniform(std::string name, vec2 v) {
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniform2f(uniform, v[0], v[1]);
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::setUniform(std::string name, vec3 v) {
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniform3f(uniform, v[0], v[1], v[2]);
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::setUniform(std::string name, vec4 v) {
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniform4f(uniform, v[0], v[1], v[2], v[3]);
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::setUniform(std::string name, mat4 m) {
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(uniform, 1, GL_FALSE, &m[0][0]);
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::setTexture(std::string name, Texture tex, int texUnit) {
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniform1i(uniform, texUnit);
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::enable() {
    glUseProgram(program);
    Engine::dieIfOpenGLError();
}

inline void ShaderProgram::disable() {
    glUseProgram(0);
    Engine::dieIfOpenGLError();
}

#endif
