/*!
 * \file include/system/WindowSDL.h
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#ifndef _WINDOW_SDL_H_
#define _WINDOW_SDL_H_

#include <glm/vec2.hpp>

#include "SDL.h"

class WindowSDL {
  public:
    static int initialize();
    static void eventHandling();
    static void swapBuffers();
    static void shutdown();

    static void setSize(glm::vec2 s);
    static glm::vec2 getSize() { return size; }

    static void setFullscreen(bool f);
    static bool getFullscreen() { return fullscreen; }

    static void setMousegrab(bool g);
    static bool getMousegrab() { return mousegrab; }

    static void setTextInput(bool t);
    static bool getTextInput() { return textinput; }

  private:
    static glm::vec2 size;
    static bool fullscreen;
    static bool mousegrab;
    static bool textinput;
    static SDL_Window* window;
    static SDL_GLContext context;
    static SDL_GameController* controller;
};

#endif

