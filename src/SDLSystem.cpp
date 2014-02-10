/*!
 * \file src/SDLSystem.cpp
 * \brief SDL System interface implementation
 *
 * \author Mongoose
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#ifdef MEMORY_TEST
#include <memory_test.h>
#endif

#include <SDL2/SDL_opengl.h>

#include <MatMath.h>
#include <SDLSystem.h>

unsigned int *gWidth = 0x0;
unsigned int *gHeight = 0x0;

SDLSystem::SDLSystem() : System() {
    mWindow = 0x0;
    gWidth = &m_width;
    gHeight = &m_height;
    mFirstMouseEvent = false;
    mFullscreen = false;
}

SDLSystem::~SDLSystem() {
}

unsigned int SDLSystem::getTicks() {
    return SDL_GetTicks();
}

#ifdef FIXME
void SDLSystem::bindKeyCommand(const char *cmd, int key, int event) {
    if (key > 255) {
        printf("Currently key event mapping only handles ASCII characters\n");
        return;
    }

    printf("Bound command '%s' -> event %i (0x%x key)\n", cmd, event, key);

    keyEvents[key] = event;
}
#endif

void SDLSystem::setGrabMouse(bool on) {
    SDL_SetRelativeMouseMode(on ? SDL_TRUE : SDL_FALSE);
}

void SDLSystem::initVideo(unsigned int width, unsigned int height, bool fullscreen) {
    int flags = 0; //, x, y;

    // Create GL context
    SDL_Init(SDL_INIT_VIDEO);
    printf("Created OpenGL Context\n");
    atexit(SDL_Quit);

    m_width = width;
    m_height = height;

#ifndef __APPLE__
    if (!m_driver || !m_driver[0] || SDL_GL_LoadLibrary(m_driver) < 0) {
        SDL_ClearError();

        // Fallback 1
        if (SDL_GL_LoadLibrary("libGL.so") < 0) {
            SDL_ClearError();

            // Fallback 2
            if (SDL_GL_LoadLibrary("libGL.so.1") < 0) {
                fprintf(stderr, "initVideo> SDL_GL_LoadLibrary failed!\n");
                fprintf(stderr, "initVideo> Error is [%s].\n", SDL_GetError());
                shutdown(1);
            }
        }
    }
#endif

    flags |= SDL_WINDOW_OPENGL;

    mFullscreen = fullscreen;
    if (mFullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    setGrabMouse(true); // Always grab mouse!

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    mWindow = SDL_CreateWindow(VERSION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, flags);
    mGLContext = SDL_GL_CreateContext(mWindow);

    //SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    //SDL_EnableKeyRepeat(100, SDL_DEFAULT_REPEAT_INTERVAL);

#ifdef UNICODE_SUPPORT
    //@JML get Unicode value of key for international keyboard
    SDL_EnableUNICODE(1);
#endif

    // Start game renderer
    initGL();

    // Resize context
    resizeGL(width, height);
}

void SDLSystem::resize(unsigned int width, unsigned int height) {
    GLfloat aspect;

    m_width = width;
    m_height = height;

    aspect = (GLfloat)width/(GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // gluPerspective is deprecated!
    // gluPerspective(m_fovY, aspect, m_clipNear, m_clipFar);
    // fix: http://stackoverflow.com/a/2417756
    GLfloat fH = tanf(m_fovY * HEL_PI / 360.0f) * m_clipNear;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, m_clipNear, m_clipFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Resize context
    resizeGL(width, height);
}

void SDLSystem::runGame() {
    SDL_Event event;
    unsigned int mkeys, mod, key;
    int btn = 0;
    bool specialKey;

    for (;;) {
        // Pause for 10-20 ms
        SDL_Delay(10);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    shutdown(0);
                    break;
                case SDL_MOUSEMOTION:
                    // Wrap motion
                    if (!mFirstMouseEvent) {
                        mFirstMouseEvent = true;
                    } else {
                        handleMouseMotionEvent(event.motion.xrel/2, event.motion.yrel/2);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    //handleMouseEvent(event.button.button, event.button.state,
                    //                    event.button.x, event.button.y);

                    switch (event.button.button) {
                        case SDL_BUTTON_LEFT:
                            btn = SYS_MOUSE_LEFT;
                            break;
                        case SDL_BUTTON_RIGHT:
                            btn = SYS_MOUSE_RIGHT;
                            break;
                        case SDL_BUTTON_MIDDLE:
                            btn = SYS_MOUSE_MIDDLE;
                            break;
                    }

                    if (event.button.state == SDL_PRESSED) {
                        handleKeyPressEvent(btn, 0); //! \fixme mod not used
                    } else {
                        handleKeyReleaseEvent(btn, 0); //! \fixme mod not used
                    }
                    break;
                case SDL_KEYUP:
                case SDL_KEYDOWN:
                    //SDL_GetMouseState(&x, &y); // Get cursor pos

                    mkeys = (unsigned int)SDL_GetModState();
                    mod = 0;

                    if (mkeys & KMOD_LSHIFT)
                        mod |= SYS_MOD_KEY_LSHIFT;

                    if (mkeys & KMOD_RSHIFT)
                        mod |= SYS_MOD_KEY_RSHIFT;

                    if (mkeys & KMOD_LCTRL)
                        mod |= SYS_MOD_KEY_LCTRL;

                    if (mkeys & KMOD_RCTRL)
                        mod |= SYS_MOD_KEY_RCTRL;

                    if (mkeys & KMOD_LALT)
                        mod |= SYS_MOD_KEY_LALT;

                    if (mkeys & KMOD_RALT)
                        mod |= SYS_MOD_KEY_RALT;

                    if (mkeys & KMOD_LGUI)
                        mod |= SYS_MOD_KEY_LMETA;

                    if (mkeys & KMOD_RGUI)
                        mod |= SYS_MOD_KEY_RMETA;

                    key = event.key.keysym.sym;
                    specialKey = false;

                    switch (key) {
                        case SDLK_F1:
                            key = SYS_KEY_F1;
                            specialKey = true;
                            break;
                        case SDLK_F2:
                            key = SYS_KEY_F2;
                            specialKey = true;
                            break;
                        case SDLK_F3:
                            key = SYS_KEY_F3;
                            specialKey = true;
                            break;
                        case SDLK_F4:
                            key = SYS_KEY_F4;
                            specialKey = true;
                            break;
                        case SDLK_F5:
                            key = SYS_KEY_F5;
                            specialKey = true;
                            break;
                        case SDLK_F6:
                            key = SYS_KEY_F6;
                            specialKey = true;
                            break;
                        case SDLK_F7:
                            key = SYS_KEY_F7;
                            specialKey = true;
                            break;
                        case SDLK_F8:
                            key = SYS_KEY_F8;
                            specialKey = true;
                            break;
                        case SDLK_F9:
                            key = SYS_KEY_F9;
                            specialKey = true;
                            break;
                        case SDLK_F10:
                            key = SYS_KEY_F10;
                            specialKey = true;
                            break;
                        case SDLK_F11:
                            key = SYS_KEY_F11;
                            specialKey = true;
                            break;
                        case SDLK_F12:
                            key = SYS_KEY_F12;
                            specialKey = true;
                            break;
                        case SDLK_UP:
                            key = SYS_KEY_UP;
                            specialKey = true;
                            break;
                        case SDLK_DOWN:
                            key = SYS_KEY_DOWN;
                            specialKey = true;
                            break;
                        case SDLK_RIGHT:
                            key = SYS_KEY_RIGHT;
                            specialKey = true;
                            break;
                        case SDLK_LEFT:
                            key = SYS_KEY_LEFT;
                            specialKey = true;
                            break;
                        case SDLK_PAGEDOWN:
                            key = SYS_KEY_PAGEDOWN;
                            specialKey = true;
                            break;
                        case SDLK_PAGEUP:
                            key = SYS_KEY_PAGEUP;
                            specialKey = true;
                            break;
                    }

#ifdef __APPLE__
                    // Handle CMD+Q to quit in all circumstances
                    if (key == 'q') {
                        if (mod & SYS_MOD_KEY_LMETA) {
                            shutdown(0);
                        }
                    }
#endif

#ifdef UNICODE_SUPPORT
                    // JML: if a std key was pressed get it ascii code
                    if (!specialKey && key != 0) {
                        if ((event.key.keysym.unicode & 0xFF80) == 0) {
                            key = (unsigned int)(event.key.keysym.unicode & 0x7F);
                        } else {
                            key = 0;
                        }
                    }
#else
                    /*! \fixme Avoid passing modifers as a key, since the
                     * consoles using this expect text characters, add unicode
                     * support later when they're able to handle it
                     */
                    if (key > 255 && key < 1000) {
                        key = 0;
                    }
#endif

                    if (key == mConsoleKey) {
                        if (event.type == SDL_KEYDOWN) {
                            mConsoleMode = !mConsoleMode;
                            // Tmp hack
                            handleConsoleKeyPressEvent(mConsoleKey, 0);
                        }
                    } else if (mConsoleMode) { // Console keying (text input)
                        switch (event.type) {
                            case SDL_KEYDOWN:
                                handleConsoleKeyPressEvent(key, mod);
                                break;
                            default:
                                ;
                        }
                    } else if (mKeyEvents[key] != 0) { // Bound key
                        //if (key < 255 && mKeyEvents[key] != 0)
                        key = mKeyEvents[key];

                        switch (event.type) {
                            case SDL_KEYDOWN:
                                handleBoundKeyPressEvent(key);
                                break;
                            default:
                                handleBoundKeyReleaseEvent(key);
                        }
                    } else { // 'Classic' key event handlers
                        switch (event.type) {
                            case SDL_KEYDOWN:
                                handleKeyPressEvent(key, mod);
                                break;
                            default:
                                handleKeyReleaseEvent(key, mod);
                        }
                    }
                    break;
                case SDL_WINDOWEVENT:
                    switch(event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            resizeGL(event.window.data1, event.window.data2);
                            gameFrame();
                            break;
                    }
                    break;
            }
        }

        // Game frame
        gameFrame();
    }
}

void SDLSystem::shutdown(int i) {
    //SDL_QuitSubSystem(SDL_OPENGL);
    //SDL_Quit(); // Moved to atexit() call

    //#ifdef DEBUG_MEMORY
    //printf("[Mongoose MEMORY_DEBUG]\nUnfreed memory table:\n");
    //dump_memory_report();
    //#endif

    exit(i);
}

void SDLSystem::toggleFullscreen() {
    if (mWindow) {
        mFullscreen = !mFullscreen;

        if (mFullscreen)
            SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        else
            SDL_SetWindowFullscreen(mWindow, 0);

        // resize(width, height); // not needed with desktop fullscreen
    }
}

void SDLSystem::swapBuffersGL() {
    SDL_GL_SwapWindow(mWindow);
}

