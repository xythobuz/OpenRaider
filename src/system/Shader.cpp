/*!
 * \file src/system/Shader.cpp
 * \brief OpenGL Shader implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "TextureManager.h"
#include "system/Window.h"
#include "system/Shader.h"

ShaderBuffer::~ShaderBuffer() {
    if (created)
        glDeleteBuffers(1, &buffer);
}

void ShaderBuffer::bufferData(int elem, int size, void* data) {
    if (!created) {
        glGenBuffers(1, &buffer);
        created = true;
    }

    boundSize = elem;
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, elem * size, data, GL_STATIC_DRAW);
}

void ShaderBuffer::bindBuffer() {
    if (!created) {
        glGenBuffers(1, &buffer);
        created = true;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void ShaderBuffer::bindBuffer(int location, int size) {
    if (!created) {
        glGenBuffers(1, &buffer);
        created = true;
    }

    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderBuffer::unbind(int location) {
    assert(created == true);
    glDisableVertexAttribArray(location);
}

// ----------------------------------------------------------------------------

Shader::~Shader() {
    if (programID >= 0)
        glDeleteProgram(programID);
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

void Shader::loadUniform(int uni, glm::vec2 vec) {
    glUniform2f(getUniform(uni), vec.x, vec.y);
}

void Shader::loadUniform(int uni, glm::vec4 vec) {
    glUniform4f(getUniform(uni), vec.r, vec.g, vec.b, vec.a);
}

void Shader::loadUniform(int uni, glm::mat4 mat) {
    glUniformMatrix4fv(getUniform(uni), 1, GL_FALSE, &mat[0][0]);
}

void Shader::loadUniform(int uni, int texture, TextureManager::TextureStorage store) {
    glUniform1i(getUniform(uni), getTextureManager().bindTexture(texture, store));
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

// ----------------------------------------------------------------------------

Shader Shader::textShader;
Shader Shader::textureShader;
Shader Shader::colorShader;
unsigned int Shader::vertexArrayID = 0;

int Shader::initialize() {
    getLog() << "GL Ven.: " << glGetString(GL_VENDOR) << Log::endl;
    getLog() << "GL Ren.: " << glGetString(GL_RENDERER) << Log::endl;
    getLog() << "GL Ver.: " << glGetString(GL_VERSION) << Log::endl;
    getLog() << "GLSL V.: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << Log::endl;

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Set background to black
    //glClearColor(BLACK[0] / 256.0f, BLACK[1] / 256.0f, BLACK[2] / 256.0f, BLACK[3] / 256.0f);
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    // Set up Z buffer
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if closer to camera
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up culling
    //glEnable(GL_CULL_FACE); //! \todo Transparency?

    glPointSize(5.0f);

    if (textShader.compile(textShaderVertex, textShaderFragment) < 0)
        return -1;
    if (textShader.addUniform("screen") < 0)
        return -2;
    if (textShader.addUniform("textureSampler") < 0)
        return -3;
    if (textShader.addUniform("colorVar") < 0)
        return -4;

    if (textureShader.compile(textureShaderVertex, textureShaderFragment) < 0)
        return -5;
    if (textureShader.addUniform("MVP") < 0)
        return -6;
    if (textureShader.addUniform("textureSampler") < 0)
        return -7;

    if (colorShader.compile(colorShaderVertex, colorShaderFragment) < 0)
        return -8;
    if (colorShader.addUniform("MVP") < 0)
        return -9;

    return 0;
}

void Shader::shutdown() {
    glDeleteVertexArrays(1, &vertexArrayID);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& uvs, glm::vec4 color,
                    unsigned int texture, TextureManager::TextureStorage store) {
    assert(vertices.getSize() == uvs.getSize());
    assert((vertices.getSize() % 3) == 0);

    textShader.use();
    textShader.loadUniform(0, Window::getSize());
    textShader.loadUniform(1, texture, store);
    textShader.loadUniform(2, color);
    vertices.bindBuffer(0, 2);
    uvs.bindBuffer(1, 2);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, vertices.getSize());
    glEnable(GL_DEPTH_TEST);

    vertices.unbind(0);
    uvs.unbind(1);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& uvs, unsigned int texture,
                    glm::mat4 MVP, TextureManager::TextureStorage store) {
    assert(vertices.getSize() == uvs.getSize());
    assert((vertices.getSize() % 3) == 0);

    textureShader.use();
    textureShader.loadUniform(0, MVP);
    textureShader.loadUniform(1, texture, store);
    vertices.bindBuffer(0, 3);
    uvs.bindBuffer(1, 2);
    glDrawArrays(GL_TRIANGLES, 0, vertices.getSize());
    vertices.unbind(0);
    uvs.unbind(1);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& uvs, ShaderBuffer& indices,
                    unsigned int texture, glm::mat4 MVP, TextureManager::TextureStorage store) {
    assert(vertices.getSize() == uvs.getSize());
    assert((indices.getSize() % 3) == 0);

    textureShader.use();
    textureShader.loadUniform(0, MVP);
    textureShader.loadUniform(1, texture, store);
    vertices.bindBuffer(0, 3);
    uvs.bindBuffer(1, 2);
    indices.bindBuffer();
    glDrawElements(GL_TRIANGLES, indices.getSize(), GL_UNSIGNED_SHORT, nullptr);
    vertices.unbind(0);
    uvs.unbind(1);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& colors, glm::mat4 MVP, unsigned int mode) {
    assert(vertices.getSize() == colors.getSize());

    colorShader.use();
    colorShader.loadUniform(0, MVP);
    vertices.bindBuffer(0, 3);
    colors.bindBuffer(1, 3);
    glDrawArrays(mode, 0, vertices.getSize());
    vertices.unbind(0);
    colors.unbind(1);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& colors, ShaderBuffer& indices,
                    glm::mat4 MVP, unsigned int mode) {
    assert(vertices.getSize() == colors.getSize());

    colorShader.use();
    colorShader.loadUniform(0, MVP);
    vertices.bindBuffer(0, 3);
    colors.bindBuffer(1, 3);
    indices.bindBuffer();
    glDrawElements(mode, indices.getSize(), GL_UNSIGNED_SHORT, nullptr);
    vertices.unbind(0);
    colors.unbind(1);
}

// --------------------------------------
// *INDENT-OFF*

const char* Shader::textShaderVertex = R"!?!(
#version 330 core

layout(location = 0) in vec2 vertexPosition_screen;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform vec2 screen;

void main() {
    vec2 halfScreen = screen / 2;
    vec2 vertexPosition_homogenous = (vertexPosition_screen - halfScreen) / halfScreen;

    gl_Position = vec4(vertexPosition_homogenous.x, -vertexPosition_homogenous.y, 0, 1);
    UV = vertexUV;
}
)!?!";

const char* Shader::textShaderFragment = R"!?!(
#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D textureSampler;
uniform vec4 colorVar;

void main() {
    color = texture(textureSampler, UV) * colorVar;
}
)!?!";

// --------------------------------------

const char* Shader::textureShaderVertex = R"!?!(
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform mat4 MVP;

void main() {
    vec4 pos = MVP * vec4(vertexPosition_modelspace.x,
                          -vertexPosition_modelspace.y,
                          vertexPosition_modelspace.z,
                          1);
    gl_Position = vec4(-pos.x, pos.yzw);
    UV = vertexUV;
}
)!?!";

const char* Shader::textureShaderFragment = R"!?!(
#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D textureSampler;

void main() {
    color = texture(textureSampler, UV);
}
)!?!";

// --------------------------------------

const char* Shader::colorShaderVertex = R"!?!(
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

out vec3 Color;

uniform mat4 MVP;

void main() {
    vec4 pos = MVP * vec4(vertexPosition_modelspace.x,
                          -vertexPosition_modelspace.y,
                          vertexPosition_modelspace.z,
                          1);
    gl_Position = vec4(-pos.x, pos.yzw);
    Color = vertexColor;
}
)!?!";

const char* Shader::colorShaderFragment = R"!?!(
#version 330 core

in vec3 Color;

out vec4 color;

void main() {
    color = vec4(Color, 1);
}
)!?!";

// --------------------------------------
// *INDENT-ON*

