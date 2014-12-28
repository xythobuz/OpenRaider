/*!
 * \file src/system/WindowSDL.cpp
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "Log.h"
#include "RunTime.h"
#include "UI.h"
#include "utils/strings.h"
#include "system/WindowSDL.h"

#define SUBSYSTEMS_USED (SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)

glm::vec2 WindowSDL::size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
bool WindowSDL::fullscreen = false;
bool WindowSDL::mousegrab = false;
bool WindowSDL::textinput = false;
SDL_Window* WindowSDL::window = nullptr;
SDL_GLContext WindowSDL::context = nullptr;
SDL_GameController* WindowSDL::controller = nullptr;

int WindowSDL::initialize() {
    if (SDL_Init(SUBSYSTEMS_USED) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    if ((SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1) != 0)
        //|| (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0)
        || (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))) {
        std::cout << "SDL_GL_SetAttribute Error: " << SDL_GetError() << std::endl;
        return -2;
    }

    window = SDL_CreateWindow(VERSION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              size.x, size.y, flags);
    if (!window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return -3;
    }

    context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        return -4;
    }

    setMousegrab(mousegrab);
    setTextInput(textinput);

    if (SDL_NumJoysticks() == 0) {
        getLog() << "No Joystick found!" << Log::endl;
        return 0;
    }

    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                getLog() << "Using controller \"" << SDL_GameControllerName(controller)
                         << "\"..." << Log::endl;
                break;
            } else {
                getLog() << "Couldn't open controller \"" << SDL_GameControllerNameForIndex(i)
                         << "\"!" << Log::endl;
            }
        } else {
            getLog() << "Joystick \"" << SDL_JoystickNameForIndex(i)
                     << "\" is no controller!" << Log::endl;
        }
    }

    return 0;
}

void WindowSDL::eventHandling() {
    SDL_Event event;
    KeyboardButton button;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_CONTROLLERAXISMOTION:
                button = unknownKey;
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
                    button = leftXAxis;
                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
                    button = leftYAxis;
                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
                    button = rightXAxis;
                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
                    button = rightYAxis;
                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
                    button = leftTrigger;
                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
                    button = rightTrigger;
                if (button != unknownKey)
                    UI::handleControllerAxis(event.caxis.value / 32768.0f, button);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                button = unknownKey;
                if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A)
                    button = aButton;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B)
                    button = bButton;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_X)
                    button = xButton;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_Y)
                    button = yButton;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
                    button = backButton;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE)
                    button = guideButton;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START)
                    button = startButton;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
                    button = leftStick;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
                    button = rightStick;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
                    button = leftShoulder;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
                    button = rightShoulder;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)
                    button = padUp;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
                    button = padDown;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                    button = padLeft;
                else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
                    button = padRight;
                if (button != unknownKey)
                    UI::handleControllerButton(button, (event.type == SDL_CONTROLLERBUTTONUP));
                break;

            case SDL_MOUSEMOTION:
                UI::handleMouseMotion(event.motion.xrel, event.motion.yrel, event.motion.x, event.motion.y);
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                button = unknownKey;
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
                if (button != unknownKey)
                    UI::handleMouseClick(event.button.x, event.button.y, button,
                                         (event.type == SDL_MOUSEBUTTONUP));
                break;

            case SDL_MOUSEWHEEL:
                if ((event.wheel.x != 0) || (event.wheel.y != 0))
                    UI::handleMouseScroll(event.wheel.x, event.wheel.y);
                break;

            case SDL_TEXTINPUT:
            case SDL_TEXTEDITING:
                if (event.text.text != nullptr)
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
                    setSize(glm::vec2(event.window.data1, event.window.data2));
                break;

            case SDL_QUIT:
                RunTime::setRunning(false);
                break;
        }
    }

    UI::eventsFinished();
}

void WindowSDL::swapBuffers() {
    SDL_GL_SwapWindow(window);
}

void WindowSDL::shutdown() {
    if (context) {
        SDL_GL_DeleteContext(context);
        context = nullptr;
    }

    if (controller) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        SDL_QuitSubSystem(SUBSYSTEMS_USED);
        SDL_Quit();
        window = nullptr;
    }
}

void WindowSDL::setSize(glm::vec2 s) {
    assert((s.x > 0) && (s.y > 0));

    size = s;
    if (window)
        SDL_SetWindowSize(window, size.x, size.y);
}

void WindowSDL::setFullscreen(bool f) {
    fullscreen = f;
    if (window) {
        if (fullscreen) {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else {
            SDL_SetWindowFullscreen(window, 0);
        }
    }
}

void WindowSDL::setMousegrab(bool g) {
    mousegrab = g;
    if (window) {
        if (mousegrab) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
        } else {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_ShowCursor(1);
        }
    }
}

void WindowSDL::setTextInput(bool t) {
    textinput = t;
    if (textinput)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
}

