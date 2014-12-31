/*!
 * \file src/system/Window.cpp
 * \brief windowing implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "UI.h"
#include "system/Window.h"

#ifdef USING_SDL
#include "system/WindowSDL.h"
#elif defined(USING_GLFW)
#include "system/WindowGLFW.h"
#else
#error "No windowing library selected!"
#endif

int Window::initialize() {
    int res;
#ifdef USING_SDL
    res = WindowSDL::initialize();
#elif defined(USING_GLFW)
    res = WindowGLFW::initialize();
#else
    res = -1;
#endif
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
#ifdef USING_SDL
    WindowSDL::shutdown();
#elif defined(USING_GLFW)
    WindowGLFW::shutdown();
#endif
}

void Window::setSize(glm::i32vec2 s) {
#ifdef USING_SDL
    WindowSDL::setSize(s);
#elif defined(USING_GLFW)
    WindowGLFW::setSize(s);
#endif

    UI::setSize(s);
    Camera::setSize(s);
    glViewport(0, 0, s.x, s.y);
}

glm::i32vec2 Window::getSize() {
    glm::i32vec2 ret(-1, -1);
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
    bool ret;
#ifdef USING_SDL
    ret = WindowSDL::getFullscreen();
#elif defined(USING_GLFW)
    ret = WindowGLFW::getFullscreen();
#else
    ret = false;
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
    bool ret;
#ifdef USING_SDL
    ret = WindowSDL::getMousegrab();
#elif defined(USING_GLFW)
    ret = WindowGLFW::getMousegrab();
#else
    ret = false;
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
    bool ret;
#ifdef USING_SDL
    ret = WindowSDL::getTextInput();
#elif defined(USING_GLFW)
    ret = WindowGLFW::getTextInput();
#else
    ret = false;
#endif

    return ret;
}

