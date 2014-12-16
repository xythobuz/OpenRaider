/*!
 * \file include/Window.h
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

class Shader {
  public:
    Shader() : programID(-1) { }
    ~Shader();

    int compile(const char* vertex, const char* fragment);
    void use();

    int addUniform(const char* name);
    unsigned int getUniform(int n);

    void addBuffer(int n = 1);
    unsigned int getBuffer(int n);

  private:
    int programID;
    std::vector<unsigned int> uniforms;
    std::vector<unsigned int> buffers;
};

class Window {
  public:

    virtual ~Window() {}

    virtual void setSize(unsigned int width, unsigned int height) = 0;
    virtual unsigned int getWidth();
    virtual unsigned int getHeight();

    virtual void setFullscreen(bool fullscreen) = 0;
    virtual bool getFullscreen();

    virtual void setMousegrab(bool grab) = 0;
    virtual bool getMousegrab();

    virtual int initialize() = 0;

    virtual void eventHandling() = 0;

    virtual void setTextInput(bool on) = 0;
    virtual bool getTextInput();

    virtual void swapBuffersGL() = 0;

    static int initializeGL();
    static void shutdownGL();
    static void resizeGL();

    static void drawTextGL(std::vector<glm::vec2>& vertices, std::vector<glm::vec2>& uvs,
                           glm::vec4 color, unsigned int texture);

    static void drawGL(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs,
                       std::vector<unsigned short>& indices, glm::mat4 MVP, unsigned int texture);

  protected:
    bool mInit;
    bool mFullscreen;
    bool mMousegrab;
    bool mTextInput;
    unsigned int mWidth;
    unsigned int mHeight;

  private:
    static Shader textShader;
    static const char* textShaderVertex;
    static const char* textShaderFragment;

    static Shader imguiShader;
    static const char* imguiShaderVertex;
    static const char* imguiShaderFragment;

    static Shader textureShader;
    static const char* textureShaderVertex;
    static const char* textureShaderFragment;

    static unsigned int vertexArrayID;

    friend class UI;
};

Window& getWindow();

#endif

