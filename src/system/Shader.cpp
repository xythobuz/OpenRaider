/*!
 * \file src/system/Shader.cpp
 * \brief OpenGL Shader Implementation
 *
 * \author xythobuz
 */

#include <sstream>

#include "global.h"
#include "Log.h"
#include "system/Window.h"
#include "system/Shader.h"

#include <glbinding/gl/gl33.h>

ShaderBuffer::~ShaderBuffer() {
    if (created)
        gl::glDeleteBuffers(1, &buffer);
}

void ShaderBuffer::bufferData(int elem, int size, void* data) {
    if (!created) {
        gl::glGenBuffers(1, &buffer);
        created = true;
    }

    boundSize = elem;
    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, buffer);
    gl::glBufferData(gl::GL_ARRAY_BUFFER, elem * size, data, gl::GL_DYNAMIC_DRAW);
}

void ShaderBuffer::bindBuffer() {
    if (!created) {
        gl::glGenBuffers(1, &buffer);
        created = true;
    }

    gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void ShaderBuffer::bindBuffer(int location, int size) {
    if (!created) {
        gl::glGenBuffers(1, &buffer);
        created = true;
    }

    gl::glEnableVertexAttribArray(location);
    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, buffer);
    gl::glVertexAttribPointer(location, size, gl::GL_FLOAT, gl::GL_FALSE, 0, nullptr);
}

void ShaderBuffer::unbind(int location) {
    orAssert(created == true);
    gl::glDisableVertexAttribArray(location);
}

// ----------------------------------------------------------------------------

ShaderTexture::ShaderTexture(int w, int h) : width(w), height(h) {
    gl::glGenFramebuffers(1, &framebuffer);
    bind();

    texture = TextureManager::loadBufferSlot(nullptr, width, height, ColorMode::RGBA,
              32, TextureStorage::SYSTEM, -1, false);

    gl::glGenRenderbuffers(1, &depth);
    gl::glBindRenderbuffer(gl::GL_RENDERBUFFER, depth);
    gl::glRenderbufferStorage(gl::GL_RENDERBUFFER, gl::GL_DEPTH_COMPONENT, width, height);
    gl::glFramebufferRenderbuffer(gl::GL_FRAMEBUFFER, gl::GL_DEPTH_ATTACHMENT, gl::GL_RENDERBUFFER, depth);

    gl::glFramebufferTexture(gl::GL_FRAMEBUFFER, gl::GL_COLOR_ATTACHMENT0,
                         TextureManager::getTextureID(texture, TextureStorage::SYSTEM), 0);

    gl::GLenum drawBuffer = gl::GL_COLOR_ATTACHMENT0;
    gl::glDrawBuffers(1, &drawBuffer);

    orAssert(gl::glCheckFramebufferStatus(gl::GL_FRAMEBUFFER) == gl::GL_FRAMEBUFFER_COMPLETE);
}

ShaderTexture::~ShaderTexture() {
    gl::glDeleteRenderbuffers(1, &depth);
    gl::glDeleteFramebuffers(1,  &framebuffer);

    //! \fixme free texture slot
}

void ShaderTexture::clear() {
    bind();
    gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
}

void ShaderTexture::bind() {
    gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, framebuffer);
    gl::glViewport(0, 0, width, height);
}

// ----------------------------------------------------------------------------

Shader::~Shader() {
    if (programID >= 0)
        gl::glDeleteProgram(programID);
}

int Shader::addUniform(const char* name) {
    orAssert(programID >= 0);
    int r = gl::glGetUniformLocation(programID, name);
    if (r < 0) {
        Log::get(LOG_ERROR) << "Can't find GLSL Uniform \"" << name << "\"!" << Log::endl;
        return -1;
    }
    uniforms.push_back(r);
    return uniforms.size() - 1;
}

unsigned int Shader::getUniform(int n) {
    orAssert(n >= 0);
    orAssert(n < uniforms.size());
    return uniforms.at(n);
}

void Shader::loadUniform(int uni, glm::vec2 vec) {
    gl::glUniform2f(getUniform(uni), vec.x, vec.y);
}

void Shader::loadUniform(int uni, glm::vec4 vec) {
    gl::glUniform4f(getUniform(uni), vec.r, vec.g, vec.b, vec.a);
}

void Shader::loadUniform(int uni, glm::mat4 mat) {
    gl::glUniformMatrix4fv(getUniform(uni), 1, gl::GL_FALSE, &mat[0][0]);
}

void Shader::loadUniform(int uni, int texture, TextureStorage store) {
    gl::glUniform1i(getUniform(uni), TextureManager::bindTexture(texture, store));
}

void Shader::use() {
    orAssert(programID >= 0);
    gl::glUseProgram(programID);
}

int Shader::compile(const char* vertex, const char* fragment) {
    orAssert(vertex != nullptr);
    orAssert(fragment != nullptr);

    gl::GLuint vertexID = gl::glCreateShader(gl::GL_VERTEX_SHADER);
    gl::GLuint fragmentID = gl::glCreateShader(gl::GL_FRAGMENT_SHADER);

    gl::GLint result = gl::GLint(gl::GL_FALSE);
    gl::GLint logLength = 0;

    // Compile vertex shader
    gl::glShaderSource(vertexID, 1, &vertex, nullptr);
    gl::glCompileShader(vertexID);

    // Check vertex shader
    gl::glGetShaderiv(vertexID, gl::GL_COMPILE_STATUS, &result);
    gl::glGetShaderiv(vertexID, gl::GL_INFO_LOG_LENGTH, &logLength);
    if ((logLength > 0) && (result != gl::GLint(gl::GL_TRUE))) {
        std::vector<char> message(logLength + 1);
        gl::glGetShaderInfoLog(vertexID, logLength, nullptr, &message[0]);
        Log::get(LOG_ERROR) << "Vertex Shader compilation error:" << Log::endl;
        Log::get(LOG_ERROR) << &message[0] << Log::endl;
        gl::glDeleteShader(vertexID);
        gl::glDeleteShader(fragmentID);
        return -1;
    }

    // Compile fragment shader
    gl::glShaderSource(fragmentID, 1, &fragment, nullptr);
    gl::glCompileShader(fragmentID);

    // Check fragment shader
    gl::glGetShaderiv(fragmentID, gl::GL_COMPILE_STATUS, &result);
    gl::glGetShaderiv(fragmentID, gl::GL_INFO_LOG_LENGTH, &logLength);
    if ((logLength > 0) && (result != gl::GLint(gl::GL_TRUE))) {
        std::vector<char> message(logLength + 1);
        gl::glGetShaderInfoLog(fragmentID, logLength, nullptr, &message[0]);
        Log::get(LOG_ERROR) << "Fragment Shader compilation error:" << Log::endl;
        Log::get(LOG_ERROR) << &message[0] << Log::endl;
        gl::glDeleteShader(vertexID);
        gl::glDeleteShader(fragmentID);
        return -2;
    }

    // Link both shaders
    programID = gl::glCreateProgram();
    gl::glAttachShader(programID, vertexID);
    gl::glAttachShader(programID, fragmentID);
    gl::glLinkProgram(programID);

    // Check resulting program
    gl::glGetProgramiv(programID, gl::GL_LINK_STATUS, &result);
    gl::glGetProgramiv(programID, gl::GL_INFO_LOG_LENGTH, &logLength);
    if ((logLength > 0) && (result != gl::GLint(gl::GL_TRUE))) {
        std::vector<char> message(logLength + 1);
        gl::glGetProgramInfoLog(programID, logLength, nullptr, &message[0]);
        Log::get(LOG_ERROR) << "Shader link error:" << Log::endl;
        Log::get(LOG_ERROR) << &message[0] << Log::endl;
        gl::glDeleteShader(vertexID);
        gl::glDeleteShader(fragmentID);
        gl::glDeleteProgram(programID);
        return -3;
    }

    gl::glDeleteShader(vertexID);
    gl::glDeleteShader(fragmentID);
    return programID;
}

// ----------------------------------------------------------------------------

std::string Shader::getVersion(bool linked) {
    static std::string cache;
    static std::string cacheLinked;
    static bool cacheFilled = false;

    if (!cacheFilled) {
        std::ostringstream str;
        str << "OpenGL v" << gl::glGetString(gl::GL_VERSION);
        cache = str.str();
        str.str("");
        str << "OpenGL " << gl::glGetString(gl::GL_SHADING_LANGUAGE_VERSION) << " "
            << gl::glGetString(gl::GL_RENDERER) << " (" << gl::glGetString(gl::GL_VENDOR) << ")";
        cacheLinked = str.str();
        cacheFilled = true;
    }

    if (linked) {
        return cacheLinked;
    } else {
        return cache;
    }
}

Shader Shader::textureShader;
Shader Shader::colorShader;
unsigned int Shader::vertexArrayID = 0;
bool Shader::lastBufferWasNotFramebuffer = true;

int Shader::initialize() {
    gl::glGenVertexArrays(1, &vertexArrayID);
    gl::glBindVertexArray(vertexArrayID);

    // Set background color
    gl::glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    set2DState(false);
    gl::glDepthFunc(gl::GL_LESS);

    gl::glEnable(gl::GL_BLEND);
    gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);

    gl::glPointSize(5.0f);

    if (textureShader.compile(textureShaderVertex, textureShaderFragment) < 0)
        return -1;
    if (textureShader.addUniform("MVP") < 0)
        return -2;
    if (textureShader.addUniform("textureSampler") < 0)
        return -3;

    if (colorShader.compile(colorShaderVertex, colorShaderFragment) < 0)
        return -4;
    if (colorShader.addUniform("MVP") < 0)
        return -5;

    return 0;
}

void Shader::shutdown() {
    gl::glDeleteVertexArrays(1, &vertexArrayID);
}

void Shader::set2DState(bool on, bool depth) {
    if (on) {
        gl::glDisable(gl::GL_CULL_FACE);
        if (depth)
            gl::glDisable(gl::GL_DEPTH_TEST);
    } else {
        gl::glEnable(gl::GL_CULL_FACE);
        if (depth)
            gl::glEnable(gl::GL_DEPTH_TEST);
    }
}

void Shader::bindProperBuffer(ShaderTexture* target) {
    if ((target == nullptr) && lastBufferWasNotFramebuffer) {
        lastBufferWasNotFramebuffer = false;
        gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0);
        gl::glViewport(0, 0, Window::getSize().x, Window::getSize().y);
    } else if (target != nullptr) {
        lastBufferWasNotFramebuffer = true;
        target->bind();
    }
}

void Shader::drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                    glm::mat4 MVP, unsigned int texture, TextureStorage store,
                    gl::GLenum mode, ShaderTexture* target, Shader& shader) {
    orAssert(vertices.size() == uvs.size());
    if (mode == gl::GL_TRIANGLES)
        orAssert((vertices.size() % 3) == 0);
    bindProperBuffer(target);

    shader.use();
    shader.loadUniform(0, MVP);
    shader.loadUniform(1, texture, store);

    shader.vertexBuffer.bufferData(vertices);
    shader.otherBuffer.bufferData(uvs);

    shader.vertexBuffer.bindBuffer(0, 3);
    shader.otherBuffer.bindBuffer(1, 2);

    gl::glDrawArrays(mode, 0, shader.vertexBuffer.getSize());

    shader.vertexBuffer.unbind(0);
    shader.otherBuffer.unbind(1);
}

void Shader::drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                    std::vector<unsigned short>& indices, glm::mat4 MVP,
                    unsigned int texture, TextureStorage store,
                    gl::GLenum mode, ShaderTexture* target, Shader& shader) {
    drawGLBuffer(vertices, uvs);
    drawGLOnly(indices, MVP, texture, store, mode, target, shader);
}

void Shader::drawGLBuffer(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, Shader& shader) {
    orAssert(vertices.size() == uvs.size());

    shader.vertexBuffer.bufferData(vertices);
    shader.otherBuffer.bufferData(uvs);
}

void Shader::drawGLOnly(std::vector<unsigned short>& indices, glm::mat4 MVP,
                        unsigned int texture, TextureStorage store,
                        gl::GLenum mode, ShaderTexture* target, Shader& shader) {
    if (mode == gl::GL_TRIANGLES)
        orAssert((indices.size() % 3) == 0);
    bindProperBuffer(target);

    shader.use();
    shader.loadUniform(0, MVP);
    shader.loadUniform(1, texture, store);

    shader.indexBuffer.bufferData(indices);

    shader.vertexBuffer.bindBuffer(0, 3);
    shader.otherBuffer.bindBuffer(1, 2);
    shader.indexBuffer.bindBuffer();

    gl::glDrawElements(mode, shader.indexBuffer.getSize(), gl::GL_UNSIGNED_SHORT, nullptr);

    shader.vertexBuffer.unbind(0);
    shader.otherBuffer.unbind(1);
}

void Shader::drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors,
                    glm::mat4 MVP, gl::GLenum mode, ShaderTexture* target, Shader& shader) {
    orAssert(vertices.size() == colors.size());
    if (mode == gl::GL_TRIANGLES)
        orAssert((vertices.size() % 3) == 0);
    bindProperBuffer(target);

    shader.use();
    shader.loadUniform(0, MVP);

    shader.vertexBuffer.bufferData(vertices);
    shader.otherBuffer.bufferData(colors);

    shader.vertexBuffer.bindBuffer(0, 3);
    shader.otherBuffer.bindBuffer(1, 3);

    gl::glDrawArrays(mode, 0, shader.vertexBuffer.getSize());

    shader.vertexBuffer.unbind(0);
    shader.otherBuffer.unbind(1);
}

void Shader::drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors,
                    std::vector<unsigned short>& indices, glm::mat4 MVP,
                    gl::GLenum mode, ShaderTexture* target, Shader& shader) {
    orAssert(vertices.size() == colors.size());
    if (mode == gl::GL_TRIANGLES)
        orAssert((indices.size() % 3) == 0);
    bindProperBuffer(target);

    shader.use();
    shader.loadUniform(0, MVP);

    shader.vertexBuffer.bufferData(vertices);
    shader.otherBuffer.bufferData(colors);
    shader.indexBuffer.bufferData(indices);

    shader.vertexBuffer.bindBuffer(0, 3);
    shader.otherBuffer.bindBuffer(1, 3);
    shader.indexBuffer.bindBuffer();

    gl::glDrawElements(mode, shader.indexBuffer.getSize(), gl::GL_UNSIGNED_SHORT, nullptr);

    shader.vertexBuffer.unbind(0);
    shader.otherBuffer.unbind(1);
}

// --------------------------------------
// *INDENT-OFF*

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

