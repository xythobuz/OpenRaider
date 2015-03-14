/*!
 * \file src/system/Shader.cpp
 * \brief OpenGL Shader Implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
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

ShaderTexture::ShaderTexture(int w, int h) : width(w), height(h) {
    glGenFramebuffers(1, &framebuffer);
    bind();

    texture = TextureManager::loadBufferSlot(nullptr, width, height, ColorMode::RGBA,
              32, TextureStorage::SYSTEM, -1, false);

    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         TextureManager::getTextureID(texture, TextureStorage::SYSTEM), 0);

    GLenum drawBuffer = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &drawBuffer);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

ShaderTexture::~ShaderTexture() {
    glDeleteRenderbuffers(1, &depth);
    glDeleteFramebuffers(1,  &framebuffer);

    //! \fixme free texture slot
}

void ShaderTexture::clear() {
    bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShaderTexture::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
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
        Log::get(LOG_ERROR) << "Can't find GLSL Uniform \"" << name << "\"!" << Log::endl;
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

void Shader::loadUniform(int uni, int texture, TextureStorage store) {
    glUniform1i(getUniform(uni), TextureManager::bindTexture(texture, store));
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

    GLint result = GLint(GL_FALSE);
    GLint logLength = 0;

    // Compile vertex shader
    glShaderSource(vertexID, 1, &vertex, nullptr);
    glCompileShader(vertexID);

    // Check vertex shader
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &logLength);
    if ((logLength > 0) && (result != GLint(GL_TRUE))) {
        std::vector<char> message(logLength + 1);
        glGetShaderInfoLog(vertexID, logLength, nullptr, &message[0]);
        Log::get(LOG_ERROR) << "Vertex Shader compilation error:" << Log::endl;
        Log::get(LOG_ERROR) << &message[0] << Log::endl;
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
    if ((logLength > 0) && (result != GLint(GL_TRUE))) {
        std::vector<char> message(logLength + 1);
        glGetShaderInfoLog(fragmentID, logLength, nullptr, &message[0]);
        Log::get(LOG_ERROR) << "Fragment Shader compilation error:" << Log::endl;
        Log::get(LOG_ERROR) << &message[0] << Log::endl;
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
    if ((logLength > 0) && (result != GLint(GL_TRUE))) {
        std::vector<char> message(logLength + 1);
        glGetProgramInfoLog(programID, logLength, nullptr, &message[0]);
        Log::get(LOG_ERROR) << "Shader link error:" << Log::endl;
        Log::get(LOG_ERROR) << &message[0] << Log::endl;
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
    Log::get(LOG_DEBUG) << "GL Ven.: " << glGetString(GL_VENDOR) << Log::endl;
    Log::get(LOG_DEBUG) << "GL Ren.: " << glGetString(GL_RENDERER) << Log::endl;
    Log::get(LOG_DEBUG) << "GL Ver.: " << glGetString(GL_VERSION) << Log::endl;
    Log::get(LOG_DEBUG) << "GLSL V.: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << Log::endl;

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Set background color
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    set2DState(false);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

void Shader::set2DState(bool on, bool depth) {
    if (on) {
        glDisable(GL_CULL_FACE);
        if (depth)
            glDisable(GL_DEPTH_TEST);
    } else {
        glEnable(GL_CULL_FACE);
        if (depth)
            glEnable(GL_DEPTH_TEST);
    }
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& uvs, glm::vec4 color,
                    unsigned int texture, TextureStorage store, GLenum mode,
                    ShaderTexture* target, Shader& shader) {
    assert(vertices.getSize() == uvs.getSize());
    if (mode == GL_TRIANGLES) {
        assert((vertices.getSize() % 3) == 0);
    }

    if (target == nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getSize().x, Window::getSize().y);
    } else {
        target->bind();
    }

    shader.use();
    shader.loadUniform(0, glm::vec2(Window::getSize()));
    shader.loadUniform(1, texture, store);
    shader.loadUniform(2, color);
    vertices.bindBuffer(0, 2);
    uvs.bindBuffer(1, 2);

    set2DState(true);
    glDrawArrays(mode, 0, vertices.getSize());
    set2DState(false);

    vertices.unbind(0);
    uvs.unbind(1);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& uvs, unsigned int texture,
                    glm::mat4 MVP, TextureStorage store, ShaderTexture* target,
                    Shader& shader) {
    assert(vertices.getSize() == uvs.getSize());
    assert((vertices.getSize() % 3) == 0);

    if (target == nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getSize().x, Window::getSize().y);
    } else {
        target->bind();
    }

    shader.use();
    shader.loadUniform(0, MVP);
    shader.loadUniform(1, texture, store);
    vertices.bindBuffer(0, 3);
    uvs.bindBuffer(1, 2);
    glDrawArrays(GL_TRIANGLES, 0, vertices.getSize());
    vertices.unbind(0);
    uvs.unbind(1);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& uvs, ShaderBuffer& indices,
                    unsigned int texture, glm::mat4 MVP, TextureStorage store,
                    ShaderTexture* target, Shader& shader) {
    assert(vertices.getSize() == uvs.getSize());
    assert((indices.getSize() % 3) == 0);

    if (target == nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getSize().x, Window::getSize().y);
    } else {
        target->bind();

        unsigned int sz = vertices.getSize();
        glm::vec3* buffer = new glm::vec3[sz];
        glBindBuffer(GL_ARRAY_BUFFER, vertices.getBuffer());
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sz * sizeof(glm::vec3), buffer);

        Log::get(LOG_DEBUG) << "drawGL Vertex dump:" << Log::endl;
        for (unsigned int i = 0; i < sz; i++) {
            glm::vec4 tmp(buffer[i], 1.0f);
            tmp = MVP * tmp;
            glm::vec3 res(tmp.x, tmp.y, tmp.z);
            Log::get(LOG_DEBUG) << buffer[i] << " -> " << res << Log::endl;
        }

        delete [] buffer;
    }

    shader.use();
    shader.loadUniform(0, MVP);
    shader.loadUniform(1, texture, store);
    vertices.bindBuffer(0, 3);
    uvs.bindBuffer(1, 2);
    indices.bindBuffer();
    glDrawElements(GL_TRIANGLES, indices.getSize(), GL_UNSIGNED_SHORT, nullptr);
    vertices.unbind(0);
    uvs.unbind(1);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& colors, glm::mat4 MVP,
                    GLenum mode, ShaderTexture* target, Shader& shader) {
    assert(vertices.getSize() == colors.getSize());
    if (mode == GL_TRIANGLES) {
        assert((vertices.getSize() % 3) == 0);
    }

    if (target == nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getSize().x, Window::getSize().y);
    } else {
        target->bind();
    }

    shader.use();
    shader.loadUniform(0, MVP);
    vertices.bindBuffer(0, 3);
    colors.bindBuffer(1, 3);
    glDrawArrays(mode, 0, vertices.getSize());
    vertices.unbind(0);
    colors.unbind(1);
}

void Shader::drawGL(ShaderBuffer& vertices, ShaderBuffer& colors, ShaderBuffer& indices,
                    glm::mat4 MVP, GLenum mode, ShaderTexture* target, Shader& shader) {
    assert(vertices.getSize() == colors.getSize());
    if (mode == GL_TRIANGLES) {
        assert((indices.getSize() % 3) == 0);
    }

    if (target == nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getSize().x, Window::getSize().y);
    } else {
        target->bind();
    }

    shader.use();
    shader.loadUniform(0, MVP);
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

layout(location = 0) out vec4 color;

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

layout(location = 0) out vec4 color;

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

out vec3 color;

uniform mat4 MVP;

void main() {
    vec4 pos = MVP * vec4(vertexPosition_modelspace.x,
                          -vertexPosition_modelspace.y,
                          vertexPosition_modelspace.z,
                          1);
    gl_Position = vec4(-pos.x, pos.yzw);
    color = vertexColor;
}
)!?!";

const char* Shader::colorShaderFragment = R"!?!(
#version 330 core

in vec3 color;

layout(location = 0) out vec4 color_out;

void main() {
    color_out = vec4(color, 1);
}
)!?!";

// --------------------------------------
// *INDENT-ON*

