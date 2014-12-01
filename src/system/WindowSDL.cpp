/*!
 * \file src/WindowSDL.cpp
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "RunTime.h"
#include "UI.h"
#include "utils/strings.h"
#include "system/WindowSDL.h"

#define SUBSYSTEMS_USED (SDL_INIT_VIDEO | SDL_INIT_EVENTS)

WindowSDL::WindowSDL() {
    mInit = false;
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    mFullscreen = false;
    mMousegrab = false;
    mTextInput = false;
    mWindow = NULL;
    mGLContext = NULL;
}

WindowSDL::~WindowSDL() {
    if (mInit) {
        SDL_QuitSubSystem(SUBSYSTEMS_USED);
        SDL_Quit();
    }
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
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (mFullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    if ((SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0)) {
        std::cout << "SDL_GL_SetAttribute Error: " << SDL_GetError() << std::endl;
        return -2;
    }

    mWindow = SDL_CreateWindow(VERSION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               mWidth, mHeight, flags);
    if (mWindow == NULL) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return -3;
    }

    mGLContext = SDL_GL_CreateContext(mWindow);
    if (mGLContext == NULL) {
        std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        return -4;
    }

    mInit = true;
    setSize(mWidth, mHeight);
    setMousegrab(mMousegrab);

    return 0;
}

void WindowSDL::eventHandling() {
    SDL_Event event;

    assert(mInit == true);

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                UI::handleMouseMotion(event.motion.xrel, event.motion.yrel, event.motion.x, event.motion.y);
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                KeyboardButton button;
                if (event.button.button == SDL_BUTTON_LEFT)
                    button = leftmouseKey;
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    button = rightmouseKey;
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                    button = middlemouseKey;
                else if (event.button.button == SDL_BUTTON_X1)
                    button = fourthmouseKey;
                else if (event.button.button == SDL_BUTTON_X2)
                    button = fifthmouseKey;
                else
                    button = unknownKey;
                UI::handleMouseClick(event.button.x, event.button.y, button, (event.type == SDL_MOUSEBUTTONUP));
                break;

            case SDL_MOUSEWHEEL:
                if ((event.wheel.x != 0) || (event.wheel.y != 0))
                    UI::handleMouseScroll(event.wheel.x, event.wheel.y);
                break;

            case SDL_TEXTINPUT:
            case SDL_TEXTEDITING:
                if (event.text.text != NULL)
                    UI::handleText(event.text.text, (event.type == SDL_TEXTEDITING));
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                KeyboardButton key;
                switch (event.key.keysym.sym) {
                    case SDLK_0:
                        key = zeroKey;
                        break;
                    case SDLK_1:
                        key = oneKey;
                        break;
                    case SDLK_2:
                        key = twoKey;
                        break;
                    case SDLK_3:
                        key = threeKey;
                        break;
                    case SDLK_4:
                        key = fourKey;
                        break;
                    case SDLK_5:
                        key = fiveKey;
                        break;
                    case SDLK_6:
                        key = sixKey;
                        break;
                    case SDLK_7:
                        key = sevenKey;
                        break;
                    case SDLK_8:
                        key = eightKey;
                        break;
                    case SDLK_9:
                        key = nineKey;
                        break;
                    case SDLK_a:
                        key = aKey;
                        break;
                    case SDLK_b:
                        key = bKey;
                        break;
                    case SDLK_c:
                        key = cKey;
                        break;
                    case SDLK_d:
                        key = dKey;
                        break;
                    case SDLK_e:
                        key = eKey;
                        break;
                    case SDLK_f:
                        key = fKey;
                        break;
                    case SDLK_g:
                        key = gKey;
                        break;
                    case SDLK_h:
                        key = hKey;
                        break;
                    case SDLK_i:
                        key = iKey;
                        break;
                    case SDLK_j:
                        key = jKey;
                        break;
                    case SDLK_k:
                        key = kKey;
                        break;
                    case SDLK_l:
                        key = lKey;
                        break;
                    case SDLK_m:
                        key = mKey;
                        break;
                    case SDLK_n:
                        key = nKey;
                        break;
                    case SDLK_o:
                        key = oKey;
                        break;
                    case SDLK_p:
                        key = pKey;
                        break;
                    case SDLK_q:
                        key = qKey;
                        break;
                    case SDLK_r:
                        key = rKey;
                        break;
                    case SDLK_s:
                        key = sKey;
                        break;
                    case SDLK_t:
                        key = tKey;
                        break;
                    case SDLK_u:
                        key = uKey;
                        break;
                    case SDLK_v:
                        key = vKey;
                        break;
                    case SDLK_w:
                        key = wKey;
                        break;
                    case SDLK_x:
                        key = xKey;
                        break;
                    case SDLK_y:
                        key = yKey;
                        break;
                    case SDLK_z:
                        key = zKey;
                        break;
                    case SDLK_QUOTE:
                        key = quoteKey;
                        break;
                    case SDLK_BACKSLASH:
                        key = backslashKey;
                        break;
                    case SDLK_BACKSPACE:
                        key = backspaceKey;
                        break;
                    case SDLK_CAPSLOCK:
                        key = capslockKey;
                        break;
                    case SDLK_COMMA:
                        key = commaKey;
                        break;
                    case SDLK_DELETE:
                        key = delKey;
                        break;
                    case SDLK_UP:
                        key = upKey;
                        break;
                    case SDLK_DOWN:
                        key = downKey;
                        break;
                    case SDLK_LEFT:
                        key = leftKey;
                        break;
                    case SDLK_RIGHT:
                        key = rightKey;
                        break;
                    case SDLK_END:
                        key = endKey;
                        break;
                    case SDLK_EQUALS:
                        key = equalsKey;
                        break;
                    case SDLK_ESCAPE:
                        key = escapeKey;
                        break;
                    case SDLK_F1:
                        key = f1Key;
                        break;
                    case SDLK_F2:
                        key = f2Key;
                        break;
                    case SDLK_F3:
                        key = f3Key;
                        break;
                    case SDLK_F4:
                        key = f4Key;
                        break;
                    case SDLK_F5:
                        key = f5Key;
                        break;
                    case SDLK_F6:
                        key = f6Key;
                        break;
                    case SDLK_F7:
                        key = f7Key;
                        break;
                    case SDLK_F8:
                        key = f8Key;
                        break;
                    case SDLK_F9:
                        key = f9Key;
                        break;
                    case SDLK_F10:
                        key = f10Key;
                        break;
                    case SDLK_F11:
                        key = f11Key;
                        break;
                    case SDLK_F12:
                        key = f12Key;
                        break;
                    case SDLK_BACKQUOTE:
                        key = backquoteKey;
                        break;
                    case SDLK_HOME:
                        key = homeKey;
                        break;
                    case SDLK_INSERT:
                        key = insertKey;
                        break;
                    case SDLK_LALT:
                        key = leftaltKey;
                        break;
                    case SDLK_LCTRL:
                        key = leftctrlKey;
                        break;
                    case SDLK_LEFTBRACKET:
                        key = leftbracketKey;
                        break;
                    case SDLK_LGUI:
                        key = leftguiKey;
                        break;
                    case SDLK_LSHIFT:
                        key = leftshiftKey;
                        break;
                    case SDLK_MINUS:
                        key = minusKey;
                        break;
                    case SDLK_NUMLOCKCLEAR:
                        key = numlockKey;
                        break;
                    case SDLK_PAGEDOWN:
                        key = pagedownKey;
                        break;
                    case SDLK_PAGEUP:
                        key = pageupKey;
                        break;
                    case SDLK_PAUSE:
                        key = pauseKey;
                        break;
                    case SDLK_PERIOD:
                        key = dotKey;
                        break;
                    case SDLK_RALT:
                        key = rightaltKey;
                        break;
                    case SDLK_RCTRL:
                        key = rightctrlKey;
                        break;
                    case SDLK_RETURN:
                    case SDLK_RETURN2:
                        key = enterKey;
                        break;
                    case SDLK_RGUI:
                        key = rightguiKey;
                        break;
                    case SDLK_RIGHTBRACKET:
                        key = rightbracketKey;
                        break;
                    case SDLK_RSHIFT:
                        key = rightshiftKey;
                        break;
                    case SDLK_SCROLLLOCK:
                        key = scrolllockKey;
                        break;
                    case SDLK_SEMICOLON:
                        key = semicolonKey;
                        break;
                    case SDLK_SLASH:
                        key = slashKey;
                        break;
                    case SDLK_SPACE:
                        key = spaceKey;
                        break;
                    case SDLK_TAB:
                        key = tabKey;
                        break;
                    default:
                        key = unknownKey;
                        break;

                }
                UI::handleKeyboard(key, (event.type == SDL_KEYDOWN));
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    setSize(event.window.data1, event.window.data2);
                break;

            case SDL_QUIT:
                getRunTime().setRunning(false);
                break;
        }
    }

    UI::eventsFinished();
}

void WindowSDL::setTextInput(bool on) {
    assert(mInit == true);

    mTextInput = on;
    if (mTextInput)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
}

void WindowSDL::swapBuffersGL() {
    assert(mInit == true);
    SDL_GL_SwapWindow(mWindow);
}

