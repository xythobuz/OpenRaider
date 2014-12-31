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
#include <glm/gtc/type_precision.hpp>

class Window {
  public:
    static int initialize();
    static void eventHandling();
    static void swapBuffers();
    static void shutdown();

    static void setSize(glm::i32vec2 s);
    static glm::i32vec2 getSize();

    static void setFullscreen(bool f);
    static bool getFullscreen();

    static void setMousegrab(bool g);
    static bool getMousegrab();

    static void setTextInput(bool t);
    static bool getTextInput();
};

#endif

