/*!
 * \file src/system/Window.cpp
 * \brief windowing implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "utils/strings.h"
#include "system/Window.h"

#ifdef USING_SDL
#include "system/WindowSDL.h"
#elif defined(USING_GLFW)
#include "system/WindowGLFW.h"
#else
#error "No windowing library selected!"
#endif

int Window::initialize() {
    int res = -1;
#ifdef USING_SDL
    res = WindowSDL::initialize();
#elif defined(USING_GLFW)
    res = WindowGLFW::initialize();
#endif

    initializeGL();
    return res;
}

void Window::eventHandling() {
#ifdef USING_SDL
    WindowSDL::eventHandling();
#elif defined(USING_GLFW)
    WindowGLFW::eventHandling();
#endif
}

void Window::swapBuffers() {
#ifdef USING_SDL
    WindowSDL::swapBuffers();
#elif defined(USING_GLFW)
    WindowGLFW::swapBuffers();
#endif
}

void Window::shutdown() {
    shutdownGL();

#ifdef USING_SDL
    WindowSDL::shutdown();
#elif defined(USING_GLFW)
    WindowGLFW::shutdown();
#endif
}

void Window::setSize(glm::vec2 s) {
#ifdef USING_SDL
    WindowSDL::setSize(s);
#elif defined(USING_GLFW)
    WindowGLFW::setSize(s);
#endif
}

glm::vec2 Window::getSize() {
    glm::vec2 ret(-1, -1);
#ifdef USING_SDL
    ret = WindowSDL::getSize();
#elif defined(USING_GLFW)
    ret = WindowGLFW::getSize();
#endif

    return ret;
}

void Window::setFullscreen(bool f) {
#ifdef USING_SDL
    WindowSDL::setFullscreen(f);
#elif defined(USING_GLFW)
    WindowGLFW::setFullscreen(f);
#endif
}

bool Window::getFullscreen() {
    bool ret = false;
#ifdef USING_SDL
    ret = WindowSDL::getFullscreen();
#elif defined(USING_GLFW)
    ret = WindowGLFW::getFullscreen();
#endif

    return ret;
}

void Window::setMousegrab(bool g) {
#ifdef USING_SDL
    WindowSDL::setMousegrab(g);
#elif defined(USING_GLFW)
    WindowGLFW::setMousegrab(g);
#endif
}

bool Window::getMousegrab() {
    bool ret = false;
#ifdef USING_SDL
    ret = WindowSDL::getMousegrab();
#elif defined(USING_GLFW)
    ret = WindowGLFW::getMousegrab();
#endif

    return ret;
}

void Window::setTextInput(bool t) {
#ifdef USING_SDL
    WindowSDL::setTextInput(t);
#elif defined(USING_GLFW)
    WindowGLFW::setTextInput(t);
#endif
}

bool Window::getTextInput() {
    bool ret = false;
#ifdef USING_SDL
    ret = WindowSDL::getTextInput();
#elif defined(USING_GLFW)
    ret = WindowGLFW::getTextInput();
#endif

    return ret;
}

// --------------------------------------

Shader Window::textShader;
Shader Window::imguiShader;
Shader Window::textureShader;
Shader Window::colorShader;
unsigned int Window::vertexArrayID = 0;

int Window::initializeGL() {
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
    textShader.addBuffer(2);

    if (imguiShader.compile(imguiShaderVertex, imguiShaderFragment) < 0)
        return -5;
    if (imguiShader.addUniform("screen") < 0)
        return -6;
    if (imguiShader.addUniform("textureSampler") < 0)
        return -7;
    imguiShader.addBuffer(3);

    if (textureShader.compile(textureShaderVertex, textureShaderFragment) < 0)
        return -8;
    if (textureShader.addUniform("MVP") < 0)
        return -9;
    if (textureShader.addUniform("textureSampler") < 0)
        return -10;
    textureShader.addBuffer(3);

    if (colorShader.compile(colorShaderVertex, colorShaderFragment) < 0)
        return -11;
    if (colorShader.addUniform("MVP") < 0)
        return -12;
    colorShader.addBuffer(3);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

void Window::shutdownGL() {
    glDeleteVertexArrays(1, &vertexArrayID);
}

void Window::drawTextGL(std::vector<glm::vec2>& vertices, std::vector<glm::vec2>& uvs,
                        glm::vec4 color, unsigned int texture) {
    assert(vertices.size() == uvs.size());
    assert((vertices.size() % 3) == 0);

    textShader.bufferData(0, vertices);
    textShader.bufferData(1, uvs);
    textShader.use();
    textShader.loadUniform(0, getSize());
    textShader.loadUniform(1, texture, TextureManager::TextureStorage::SYSTEM);
    textShader.loadUniform(2, color);
    textShader.bindBuffer(0, 0, 2);
    textShader.bindBuffer(1, 1, 2);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glEnable(GL_DEPTH_TEST);

    textShader.disableAttribs();
}

void Window::drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                    std::vector<unsigned short>& indices, glm::mat4 MVP, unsigned int texture) {
    assert(vertices.size() == uvs.size());
    assert((indices.size() % 3) == 0);

    textureShader.bufferData(0, vertices);
    textureShader.bufferData(1, uvs);
    textureShader.bufferData(2, indices);
    textureShader.use();
    textureShader.loadUniform(0, MVP);
    textureShader.loadUniform(1, texture, TextureManager::TextureStorage::GAME);
    textureShader.bindBuffer(0, 0, 3);
    textureShader.bindBuffer(1, 1, 2);
    textureShader.bindBuffer(2);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, nullptr);
    textureShader.disableAttribs();
}

void Window::drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors,
                    std::vector<unsigned short>& indices, glm::mat4 MVP, int mode) {
    assert(vertices.size() == colors.size());

    colorShader.bufferData(0, vertices);
    colorShader.bufferData(1, colors);
    colorShader.bufferData(2, indices);
    colorShader.use();
    colorShader.loadUniform(0, MVP);
    colorShader.bindBuffer(0, 0, 3);
    colorShader.bindBuffer(1, 1, 3);
    colorShader.bindBuffer(2);
    glDrawElements(mode, indices.size(), GL_UNSIGNED_SHORT, nullptr);
    colorShader.disableAttribs();
}

// --------------------------------------
// *INDENT-OFF*

const char* Window::textShaderVertex = R"!?!(
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

const char* Window::textShaderFragment = R"!?!(
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

const char* Window::imguiShaderVertex = R"!?!(
#version 330 core

layout(location = 0) in vec2 vertexPosition_screen;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec4 vertexColor;

out vec2 UV;
out vec4 FragColor;

uniform vec2 screen;

void main() {
    vec2 halfScreen = screen / 2;
    vec2 vertexPosition_homogenous = (vertexPosition_screen - halfScreen) / halfScreen;

    gl_Position = vec4(vertexPosition_homogenous.x, -vertexPosition_homogenous.y, 0, 1);
    UV = vertexUV;
    FragColor = vertexColor;
}
)!?!";

const char* Window::imguiShaderFragment = R"!?!(
#version 330 core

in vec2 UV;
in vec4 FragColor;

out vec4 color;

uniform sampler2D textureSampler;

void main() {
    color = texture(textureSampler, UV) * FragColor;
}
)!?!";

// --------------------------------------

const char* Window::textureShaderVertex = R"!?!(
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

const char* Window::textureShaderFragment = R"!?!(
#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D textureSampler;

void main() {
    color = texture(textureSampler, UV);
}
)!?!";

// --------------------------------------

const char* Window::colorShaderVertex = R"!?!(
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

const char* Window::colorShaderFragment = R"!?!(
#version 330 core

in vec3 Color;

out vec4 color;

void main() {
    color = vec4(Color, 1);
}
)!?!";

// --------------------------------------
// *INDENT-ON*

