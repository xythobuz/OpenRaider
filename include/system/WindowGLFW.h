/*!
 * \file include/WindowGLFW.h
 * \brief GLFW windowing implementation
 *
 * \author xythobuz
 */

#ifndef _WINDOW_GLFW_H_
#define _WINDOW_GLFW_H_

#include <GLFW/glfw3.h>

#include "system/Window.h"

class WindowGLFW : public Window {
  public:

    WindowGLFW();
    virtual ~WindowGLFW();

    virtual void setSize(unsigned int width, unsigned int height);

    virtual void setFullscreen(bool fullscreen);

    virtual void setMousegrab(bool grab);

    virtual int initialize();

    virtual void eventHandling();

    virtual void setTextInput(bool on);

    virtual void swapBuffersGL();

  private:
    static void errorCallback(int error, const char* desc);
    static void sizeCallback(GLFWwindow* w, int width, int height);
    static void cursorCallback(GLFWwindow* w, double xpos, double ypos);
    static void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
    static void buttonCallback(GLFWwindow* w, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* w, double xoffset, double yoffset);

    static KeyboardButton convertAsciiButton(int key);

    GLFWwindow* mWindow;
};

#endif

