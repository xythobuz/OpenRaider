/*!
 * \file include/system/Window.h
 * \brief Windowing interface
 *
 * \author xythobuz
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Shader.h"

class Window {
  public:
    static int initialize();
    static void eventHandling();
    static void swapBuffers();
    static void shutdown();

    static void setSize(glm::vec2 s);
    static glm::vec2 getSize();

    static void setFullscreen(bool f);
    static bool getFullscreen();

    static void setMousegrab(bool g);
    static bool getMousegrab();

    static void setTextInput(bool t);
    static bool getTextInput();

    static void drawTextGL(std::vector<glm::vec2>& vertices, std::vector<glm::vec2>& uvs,
                           glm::vec4 color, unsigned int texture);

    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                       std::vector<unsigned short>& indices, glm::mat4 MVP, unsigned int texture);

    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors,
                       std::vector<unsigned short>& indices, glm::mat4 MVP, int mode = GL_TRIANGLES);

  private:
    static int initializeGL();
    static void shutdownGL();

    static Shader textShader;
    static const char* textShaderVertex;
    static const char* textShaderFragment;

    static Shader imguiShader;
    static const char* imguiShaderVertex;
    static const char* imguiShaderFragment;
    friend class UI;

    static Shader textureShader;
    static const char* textureShaderVertex;
    static const char* textureShaderFragment;

    static Shader colorShader;
    static const char* colorShaderVertex;
    static const char* colorShaderFragment;

    static unsigned int vertexArrayID;
};

#endif

