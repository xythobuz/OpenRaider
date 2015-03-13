/*!
 * \file include/system/Window.h
 * \brief Windowing Interface
 *
 * \author xythobuz
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

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

    static void setClipboard(const char* s);
    static const char* getClipboard();

    static void inputPositionCallback(int x, int y);
};

#endif

