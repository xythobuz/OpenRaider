/*!
 * \file src/WindowSDL.cpp
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#include <assert.h>

#include "WindowSDL.h"

WindowSDL::WindowSDL() {

}

WindowSDL::~WindowSDL() {

}

void WindowSDL::setSize(unsigned int width, unsigned int height) {
    assert(width > 0);
    assert(height > 0);

    mWidth = width;
    mHeight = height;
}

void WindowSDL::setFullscreen(bool fullscreen) {
    mFullscreen = fullscreen;
}

void WindowSDL::setMousegrab(bool grab) {
    mMousegrab = grab;
}

int WindowSDL::initialize() {
    return -1;
}

