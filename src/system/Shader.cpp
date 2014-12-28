/*!
 * \file src/system/Shader.cpp
 * \brief OpenGL Shader implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "TextureManager.h"
#include "system/Shader.h"

Shader::~Shader() {
    if (programID >= 0)
        glDeleteProgram(programID);

    if (!buffers.empty())
        glDeleteBuffers(buffers.size(), &buffers[0]);
}

int Shader::addUniform(const char* name) {
    assert(programID >= 0);
    int r = glGetUniformLocation(programID, name);
    if (r < 0) {
        getLog() << "Can't find GLSL Uniform \"" << name << "\"!" << Log::endl;
        return -1;
    }
    uniforms.push_back(r);
    return uniforms.size() - 1;
}

unsigned int Shader::getUniform(int n) {
    assert(n >= 0);
    assert(n < uniforms.size());
    return uniforms.at(n);
}

void Shader::addBuffer(int n) {
    int s = buffers.size();
    for (int i = 0; i < n; i++)
        buffers.push_back(0);
    glGenBuffers(n, &buffers[s]);
}

unsigned int Shader::getBuffer(int n) {
    assert(n >= 0);
    assert(n < buffers.size());
    return buffers.at(n);
}

void Shader::loadUniform(int uni, glm::vec2 vec) {
    glUniform2f(getUniform(uni), vec.x, vec.y);
}

void Shader::loadUniform(int uni, glm::vec4 vec) {
    glUniform4f(getUniform(uni), vec.r, vec.g, vec.b, vec.a);
}

void Shader::loadUniform(int uni, glm::mat4 mat) {
    glUniformMatrix4fv(getUniform(uni), 1, GL_FALSE, &mat[0][0]);
}

void Shader::loadUniform(int uni, int texture, TextureManager::TextureStorage store, int slot) {
    getTextureManager().bindTextureId(texture, store, slot);
    glUniform1i(getUniform(uni), slot);
}

void Shader::bindBuffer(int buff) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getBuffer(buff));
}

void Shader::bindBuffer(int buff, int location, int size) {
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, getBuffer(buff));
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0, nullptr);

    while (attribs.size() <= location)
        attribs.push_back(false);
    attribs.at(location) = true;
}

void Shader::disableAttribs() {
    for (int i = 0; i < attribs.size(); i++) {
        if (attribs.at(i)) {
            glDisableVertexAttribArray(i);
            attribs.at(i) = false;
        }
    }
}

void Shader::use() {
    assert(programID >= 0);
    glUseProgram(programID);
}

int Shader::compile(const char* vertex, const char* fragment) {
    assert(vertex != nullptr);
    assert(fragment != nullptr);

    GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    GLint logLength = 0;

    // Compile vertex shader
    glShaderSource(vertexID, 1, &vertex, nullptr);
    glCompileShader(vertexID);

    // Check vertex shader
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::vector<char> message(logLength + 1);
        glGetShaderInfoLog(vertexID, logLength, nullptr, &message[0]);
        if (result != GL_TRUE)
            getLog() << "Vertex Shader compilation error:" << Log::endl;
        getLog() << &message[0] << Log::endl;
        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);
        return -1;
    }

    // Compile fragment shader
    glShaderSource(fragmentID, 1, &fragment, nullptr);
    glCompileShader(fragmentID);

    // Check fragment shader
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::vector<char> message(logLength + 1);
        glGetShaderInfoLog(fragmentID, logLength, nullptr, &message[0]);
        if (result != GL_TRUE)
            getLog() << "Fragment Shader compilation error:" << Log::endl;
        getLog() << &message[0] << Log::endl;
        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);
        return -2;
    }

    // Link both shaders
    programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);
    glLinkProgram(programID);

    // Check resulting program
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::vector<char> message(logLength + 1);
        glGetProgramInfoLog(programID, logLength, nullptr, &message[0]);
        if (result != GL_TRUE)
            getLog() << "Shader link error:" << Log::endl;
        getLog() << &message[0] << Log::endl;
        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);
        glDeleteProgram(programID);
        return -3;
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    return programID;
}

