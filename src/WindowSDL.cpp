/*!
 * \file src/WindowSDL.cpp
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#include <assert.h>

#include "config.h"
#include "WindowSDL.h"

WindowSDL::WindowSDL() {
    mInit = false;
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    mFullscreen = false;
    mMousegrab = false;
    mWindow = NULL;
    mGLContext = NULL;
}

WindowSDL::~WindowSDL() {

}

void WindowSDL::setSize(unsigned int width, unsigned int height) {
    assert(width > 0);
    assert(height > 0);

    mWidth = width;
    mHeight = height;

    if (mInit == true) {
        SDL_SetWindowSize(mWindow, mWidth, mHeight);
        resizeGL(mWidth, mHeight);
    }
}

void WindowSDL::setFullscreen(bool fullscreen) {
    mFullscreen = fullscreen;

    if (mInit == true) {
        if (mFullscreen)
            SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        else
            SDL_SetWindowFullscreen(mWindow, 0);
    }
}

void WindowSDL::setMousegrab(bool grab) {
    mMousegrab = grab;

    if (mInit == true) {
        if (mMousegrab) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
        } else {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_ShowCursor(1);
        }
    }
}

int WindowSDL::initialize() {
    SDL_Init(SDL_INIT_VIDEO);

    mInit = true;

    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (mFullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    setMousegrab(mMousegrab);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    mWindow = SDL_CreateWindow(VERSION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                mWidth, mHeight, flags);
    mGLContext = SDL_GL_CreateContext(mWindow);
    setSize(mWidth, mHeight);

    return 0;
}

void WindowSDL::writeString(WindowString *s) {
    assert(s != NULL);
    assert(s->text != NULL);
    assert(mInit == true);
}

