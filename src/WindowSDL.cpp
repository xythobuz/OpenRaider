/*!
 * \file src/WindowSDL.cpp
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#include <cstdio>
#include <assert.h>

#include "config.h"
#include "main.h"
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
    mTextInput = false;
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

    tempText.text = new char[256];
    tempText.color[0] = 0xFF;
    tempText.color[1] = 0xFF;
    tempText.color[2] = 0xFF;
    tempText.color[3] = 0xFF;
    tempText.scale = 1.2f;
    tempText.w = 0;
    tempText.h = 0;
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

    if (tempText.text)
        delete [] tempText.text;
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
        resizeGL();
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
                gOpenRaider->handleMouseMotion(event.motion.xrel, event.motion.yrel);
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                MouseButton button;
                if (event.button.button == SDL_BUTTON_LEFT)
                    button = leftButton;
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    button = rightButton;
                else
                    button = middleButton;
                gOpenRaider->handleMouseClick(event.button.x, event.button.y, button, (event.type == SDL_MOUSEBUTTONUP));
                break;

            case SDL_TEXTINPUT:
            case SDL_TEXTEDITING:
                gOpenRaider->handleText(event.text.text, (event.type == SDL_TEXTEDITING));
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                KeyboardButton key;
                switch (event.key.keysym.sym) {
                    case SDLK_0:
                        key = zero;
                        break;
                    case SDLK_1:
                        key = one;
                        break;
                    case SDLK_2:
                        key = two;
                        break;
                    case SDLK_3:
                        key = three;
                        break;
                    case SDLK_4:
                        key = four;
                        break;
                    case SDLK_5:
                        key = five;
                        break;
                    case SDLK_6:
                        key = six;
                        break;
                    case SDLK_7:
                        key = seven;
                        break;
                    case SDLK_8:
                        key = eight;
                        break;
                    case SDLK_9:
                        key = nine;
                        break;
                    case SDLK_a:
                        key = a;
                        break;
                    case SDLK_b:
                        key = b;
                        break;
                    case SDLK_c:
                        key = c;
                        break;
                    case SDLK_d:
                        key = d;
                        break;
                    case SDLK_e:
                        key = e;
                        break;
                    case SDLK_f:
                        key = f;
                        break;
                    case SDLK_g:
                        key = g;
                        break;
                    case SDLK_h:
                        key = h;
                        break;
                    case SDLK_i:
                        key = i;
                        break;
                    case SDLK_j:
                        key = j;
                        break;
                    case SDLK_k:
                        key = k;
                        break;
                    case SDLK_l:
                        key = l;
                        break;
                    case SDLK_m:
                        key = m;
                        break;
                    case SDLK_n:
                        key = n;
                        break;
                    case SDLK_o:
                        key = o;
                        break;
                    case SDLK_p:
                        key = p;
                        break;
                    case SDLK_q:
                        key = q;
                        break;
                    case SDLK_r:
                        key = r;
                        break;
                    case SDLK_s:
                        key = s;
                        break;
                    case SDLK_t:
                        key = t;
                        break;
                    case SDLK_u:
                        key = u;
                        break;
                    case SDLK_v:
                        key = v;
                        break;
                    case SDLK_w:
                        key = w;
                        break;
                    case SDLK_x:
                        key = x;
                        break;
                    case SDLK_y:
                        key = y;
                        break;
                    case SDLK_z:
                        key = z;
                        break;
                    case SDLK_QUOTE:
                        key = quote;
                        break;
                    case SDLK_BACKSLASH:
                        key = backslash;
                        break;
                    case SDLK_BACKSPACE:
                        key = backspace;
                        break;
                    case SDLK_CAPSLOCK:
                        key = capslock;
                        break;
                    case SDLK_COMMA:
                        key = comma;
                        break;
                    case SDLK_DELETE:
                        key = del;
                        break;
                    case SDLK_UP:
                        key = up;
                        break;
                    case SDLK_DOWN:
                        key = down;
                        break;
                    case SDLK_LEFT:
                        key = left;
                        break;
                    case SDLK_RIGHT:
                        key = right;
                        break;
                    case SDLK_END:
                        key = end;
                        break;
                    case SDLK_EQUALS:
                        key = equals;
                        break;
                    case SDLK_ESCAPE:
                        key = escape;
                        break;
                    case SDLK_F1:
                        key = f1;
                        break;
                    case SDLK_F2:
                        key = f2;
                        break;
                    case SDLK_F3:
                        key = f3;
                        break;
                    case SDLK_F4:
                        key = f4;
                        break;
                    case SDLK_F5:
                        key = f5;
                        break;
                    case SDLK_F6:
                        key = f6;
                        break;
                    case SDLK_F7:
                        key = f7;
                        break;
                    case SDLK_F8:
                        key = f8;
                        break;
                    case SDLK_F9:
                        key = f9;
                        break;
                    case SDLK_F10:
                        key = f10;
                        break;
                    case SDLK_F11:
                        key = f11;
                        break;
                    case SDLK_F12:
                        key = f12;
                        break;
                    case SDLK_BACKQUOTE:
                        key = backquote;
                        break;
                    case SDLK_HOME:
                        key = home;
                        break;
                    case SDLK_INSERT:
                        key = insert;
                        break;
                    case SDLK_LALT:
                        key = leftalt;
                        break;
                    case SDLK_LCTRL:
                        key = leftctrl;
                        break;
                    case SDLK_LEFTBRACKET:
                        key = leftbracket;
                        break;
                    case SDLK_LGUI:
                        key = leftgui;
                        break;
                    case SDLK_LSHIFT:
                        key = leftshift;
                        break;
                    case SDLK_MINUS:
                        key = minus;
                        break;
                    case SDLK_NUMLOCKCLEAR:
                        key = numlock;
                        break;
                    case SDLK_PAGEDOWN:
                        key = pagedown;
                        break;
                    case SDLK_PAGEUP:
                        key = pageup;
                        break;
                    case SDLK_PAUSE:
                        key = pause;
                        break;
                    case SDLK_PERIOD:
                        key = dot;
                        break;
                    case SDLK_RALT:
                        key = rightalt;
                        break;
                    case SDLK_RCTRL:
                        key = rightctrl;
                        break;
                    case SDLK_RETURN:
                    case SDLK_RETURN2:
                        key = enter;
                        break;
                    case SDLK_RGUI:
                        key = rightgui;
                        break;
                    case SDLK_RIGHTBRACKET:
                        key = rightbracket;
                        break;
                    case SDLK_RSHIFT:
                        key = rightshift;
                        break;
                    case SDLK_SCROLLLOCK:
                        key = scrolllock;
                        break;
                    case SDLK_SEMICOLON:
                        key = semicolon;
                        break;
                    case SDLK_SLASH:
                        key = slash;
                        break;
                    case SDLK_SPACE:
                        key = space;
                        break;
                    case SDLK_TAB:
                        key = tab;
                        break;
                    default:
                        key = unknown;
                        break;

                }
                gOpenRaider->handleKeyboard(key, (event.type == SDL_KEYDOWN));
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

void WindowSDL::setTextInput(bool on) {
    assert(mInit == true);

    mTextInput = on;
    if (mTextInput)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
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
    color.a = s->color[3];

    SDL_Surface *surface = TTF_RenderUTF8_Blended(mFont, s->text, color);
    if (surface == NULL) {
        printf("TTF_RenderUTF8_Blended Error: %s\n", TTF_GetError());
        return;
    }

    s->w = (int)((float)surface->w * s->scale);
    s->h = (int)((float)surface->h * s->scale);

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

    GLuint xMin = s->x;
    GLuint yMin = s->y;
    GLuint xMax = xMin + (int)((float)surface->w * s->scale);
    GLuint yMax = yMin + (int)((float)surface->h * s->scale);

    glColor4f(color.r / 256.0f, color.g / 256.0f, color.b / 256.0f, color.a / 256.0f);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(xMin, yMin);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(xMin, yMax);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(xMax, yMax);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(xMax, yMin);
    glEnd();

    SDL_FreeSurface(surface);
}

void WindowSDL::drawText(unsigned int x, unsigned int y, float scale, unsigned char *color, const char *s, ...) {
    va_list args;
    va_start(args, s);
    vsnprintf(tempText.text, 256, s, args);
    tempText.text[255] = '\0';
    va_end(args);

    tempText.scale = scale;
    tempText.x = x;
    tempText.y = y;
    if (color) {
        tempText.color[0] = color[0];
        tempText.color[1] = color[1];
        tempText.color[2] = color[2];
        tempText.color[3] = color[3];
    }
    writeString(&tempText);
}

