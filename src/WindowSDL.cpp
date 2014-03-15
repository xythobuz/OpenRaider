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

#define SUBSYSTEMS_USED (SDL_INIT_VIDEO | SDL_INIT_EVENTS)

WindowSDL::WindowSDL() {
    mInit = false;
    mDriver = NULL;
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    mFullscreen = false;
    mMousegrab = false;
    mWindow = NULL;
    mGLContext = NULL;
    mFontInit = false;
    mFontName = NULL;
    mFont = NULL;

#ifdef WIN32
    setDriver("libGL32.dll");
#elif !defined(__APPLE__)
    setDriver("/usr/lib/libGL.so.1");
#endif
}

WindowSDL::~WindowSDL() {
    if (mInit) {
        SDL_QuitSubSystem(SUBSYSTEMS_USED);
        SDL_Quit();
    }

    if (mFont)
        TTF_CloseFont(mFont);

    if (mFontInit)
        TTF_Quit();

    if (mDriver)
        delete [] mDriver;

    if (mFontName)
        delete [] mFontName;
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

    if (SDL_Init(SUBSYSTEMS_USED) != 0) {
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

            case SDL_QUIT:
                exit(0);
        }
    }
}

void WindowSDL::delay(clock_t ms) {
    assert(mInit == true);

    SDL_Delay(ms);
}

void WindowSDL::swapBuffersGL() {
    assert(mInit == true);

    SDL_GL_SwapWindow(mWindow);
}

void WindowSDL::setFont(const char *font) {
    assert(font != NULL);
    assert(font[0] != '\0');
    assert(mFontInit == false);

    mFontName = fullPath(font, 0);
}

int WindowSDL::initializeFont() {
    assert(mFontInit == false);
    assert(mFontName != NULL);
    assert(mFontName[0] != '\0');

    if (TTF_Init() != 0) {
        printf("Could not initialize SDL-TTF!\n");
        return -1;
    }

    mFont = TTF_OpenFont(mFontName, 24);
    if (mFont == NULL) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return -2;
    }

    glGenTextures(1, &mFontTexture);

    mFontInit = true;
    return 0;
}

void WindowSDL::writeString(WindowString *s) {
    assert(s != NULL);
    assert(s->text != NULL);
    assert(mInit == true);

    SDL_Color color;
    color.r = s->color[0];
    color.g = s->color[1];
    color.b = s->color[2];

    SDL_Surface *surface = TTF_RenderUTF8_Blended(mFont, s->text, color);
    if (surface == NULL) {
        printf("TTF_RenderUTF8_Blended Error: %s\n", TTF_GetError());
        return;
    }

    GLenum textureFormat;
    if (surface->format->BytesPerPixel == 4) {
        if (surface->format->Rmask == 0x000000FF)
            textureFormat = GL_RGBA;
        else
            textureFormat = GL_BGRA_EXT;
    } else {
        textureFormat = GL_RGB;
    }

    glBindTexture(GL_TEXTURE_2D, mFontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

    glEnter2D(mWidth, mHeight);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(s->x, s->y);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(s->x, (int)((float)surface->h * s->scale));

        glTexCoord2f(1.0f, 1.0f);
        glVertex2i((int)((float)surface->w * s->scale), (int)((float)surface->h * s->scale));

        glTexCoord2f(1.0f, 0.0f);
        glVertex2i((int)((float)surface->w * s->scale), s->y);
    glEnd();

    glExit2D();

    SDL_FreeSurface(surface);
}

