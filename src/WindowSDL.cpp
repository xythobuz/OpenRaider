/*!
 * \file src/WindowSDL.cpp
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#include <cstdio>
#include <assert.h>

#include "config.h"
#include "utils/strings.h"
#include "WindowSDL.h"

WindowSDL::WindowSDL() {
    mInit = false;
    mDriver = NULL;
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    mFullscreen = false;
    mMousegrab = false;
    mWindow = NULL;
    mGLContext = NULL;
}

WindowSDL::~WindowSDL() {
    if (mDriver)
        delete [] mDriver;
}

void WindowSDL::setDriver(const char *driver) {
    assert(driver != NULL);
    assert(driver[0] != '\0');
    assert(mInit == false);

    mDriver = bufferString("%s", driver);
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
    assert(mInit == false);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }

#ifndef __APPLE__
    assert(mDriver != NULL);
    assert(mDriver[0] != '\0');

    if (SDL_GL_LoadLibrary(mDriver) < 0) {
        SDL_ClearError();
        if (SDL_GL_LoadLibrary("libGL.so") < 0) {
            SDL_ClearError();
            if (SDL_GL_LoadLibrary("libGL.so.1") < 0) {
                printf("Could not load OpenGL driver!\n");
                printf("SDL_GL_LoadLibrary Error: %s\n", SDL_GetError());
                return -2;
            }
        }
    }
#endif

    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (mFullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    mInit = true;
    setMousegrab(mMousegrab);

    if ((SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0)) {
        printf("SDL_GL_SetAttribute Error: %s\n", SDL_GetError());
        mInit = false;
        return -3;
    }

    mWindow = SDL_CreateWindow(VERSION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                mWidth, mHeight, flags);
    if (mWindow == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        mInit = false;
        return -4;
    }

    mGLContext = SDL_GL_CreateContext(mWindow);
    if (mGLContext == NULL) {
        printf("SDL_GL_CreateContext Error: %s\n", SDL_GetError());
        mInit = false;
        return -5;
    }

    setSize(mWidth, mHeight);

    return 0;
}

void WindowSDL::eventHandling() {
    SDL_Event event;

    assert(mInit == true);

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                exit(0);
                break;

            case SDL_MOUSEMOTION:

                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:

                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:

                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    setSize(event.window.data1, event.window.data2);
                break;
        }
    }
}

void WindowSDL::writeString(WindowString *s) {
    assert(s != NULL);
    assert(s->text != NULL);
    assert(mInit == true);


}

void WindowSDL::delay(clock_t ms) {
    assert(mInit == true);

    SDL_Delay(ms);
}

void WindowSDL::swapBuffersGL() {
    assert(mInit == true);

    SDL_GL_SwapWindow(mWindow);
}

void WindowSDL::cleanup() {
    if (mInit) {
        //SDL_QuitSubSystem(SDL_OPENGL);
        SDL_Quit();
    }
}

