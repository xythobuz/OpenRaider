/*!
 * \file src/system/WindowGLFW.cpp
 * \brief GLFW windowing implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "RunTime.h"
#include "UI.h"
#include "system/Window.h"
#include "system/WindowGLFW.h"

glm::i32vec2 WindowGLFW::size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
bool WindowGLFW::fullscreen = false;
bool WindowGLFW::mousegrab = false;
bool WindowGLFW::textinput = false;
GLFWwindow* WindowGLFW::window = nullptr;
int WindowGLFW::lastMouseX = 0;
int WindowGLFW::lastMouseY = 0;
bool WindowGLFW::modShift = false;
bool WindowGLFW::modControl = false;
bool WindowGLFW::modAlt = false;
bool WindowGLFW::modSuper = false;

int WindowGLFW::initialize() {
    glfwSetErrorCallback(WindowGLFW::errorCallback);
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(size.x, size.y, VERSION,
                              fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -2;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, WindowGLFW::sizeCallback);
    glfwSetCursorPosCallback(window, WindowGLFW::cursorCallback);
    glfwSetKeyCallback(window, WindowGLFW::keyCallback);
    glfwSetMouseButtonCallback(window, WindowGLFW::buttonCallback);
    glfwSetScrollCallback(window, WindowGLFW::scrollCallback);

    return 0;
}

void WindowGLFW::eventHandling() {
    glfwPollEvents();

    if (glfwWindowShouldClose(window)) {
        RunTime::setRunning(false);
    }

    UI::eventsFinished();
}

void WindowGLFW::swapBuffers() {
    glfwSwapBuffers(window);
}

void WindowGLFW::shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

void WindowGLFW::setSize(glm::i32vec2 s) {
    assert((s.x > 0) && (s.y > 0));
    if (window) {
        if ((size.x != s.x) || (size.y != s.y)) {
            glfwSetWindowSize(window, s.x, s.y);
        }
    }
    size = s;
}

void WindowGLFW::setFullscreen(bool f) {
    fullscreen = f;
    //! \todo GLFW does not support toggling fullscreen?!
}

void WindowGLFW::setMousegrab(bool g) {
    mousegrab = g;

    if (window) {
        if (mousegrab)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void WindowGLFW::setTextInput(bool t) {
    textinput = t;
}

void WindowGLFW::errorCallback(int error, const char* desc) {
    getLog() << "GLFW Error (" << error << "): " << desc << Log::endl;
}

void WindowGLFW::sizeCallback(GLFWwindow* w, int width, int height) {
    size = glm::i32vec2(width, height);
    Window::setSize(size);
}

void WindowGLFW::cursorCallback(GLFWwindow* w, double xpos, double ypos) {
    int xrel = xpos - lastMouseX;
    int yrel = ypos - lastMouseY;
    UI::handleMouseMotion(xrel, yrel, xpos, ypos);
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void WindowGLFW::keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (((mods & GLFW_MOD_SHIFT) != 0) != modShift) {
        modShift = (mods & GLFW_MOD_SHIFT) != 0;
        UI::handleKeyboard(leftshiftKey, modShift);
    }

    if (((mods & GLFW_MOD_CONTROL) != 0) != modControl) {
        modControl = (mods & GLFW_MOD_CONTROL) != 0;
        UI::handleKeyboard(leftctrlKey, modControl);
    }

    if (((mods & GLFW_MOD_ALT) != 0) != modAlt) {
        modAlt = (mods & GLFW_MOD_ALT) != 0;
        UI::handleKeyboard(leftaltKey, modAlt);
    }

    if (((mods & GLFW_MOD_SUPER) != 0) != modSuper) {
        modSuper = (mods & GLFW_MOD_SUPER) != 0;
        UI::handleKeyboard(leftguiKey, modSuper);
    }

    if (textinput && (action != GLFW_RELEASE)) {
        //! \todo GLFW does not support UTF8 text input?!
        if ((key >= '0') && (key <= '9')) {
            char s[2] = { (char)key, '\0' };
            UI::handleText(s, false);
        } else if ((key >= 'A') && (key <= 'Z')) {
            key = key - 'A' + 'a';
            char s[2] = { (char)key, '\0' };
            UI::handleText(s, false);
            key = key - 'a' + 'A';
        }
    }

    KeyboardButton b = convertAsciiButton(key);
    UI::handleKeyboard(b, (action != GLFW_RELEASE));
}

void WindowGLFW::buttonCallback(GLFWwindow* w, int button, int action, int mods) {
    if (((mods & GLFW_MOD_SHIFT) != 0) != modShift) {
        modShift = (mods & GLFW_MOD_SHIFT) != 0;
        UI::handleKeyboard(leftshiftKey, modShift);
    }

    if (((mods & GLFW_MOD_CONTROL) != 0) != modControl) {
        modControl = (mods & GLFW_MOD_CONTROL) != 0;
        UI::handleKeyboard(leftctrlKey, modControl);
    }

    if (((mods & GLFW_MOD_ALT) != 0) != modAlt) {
        modAlt = (mods & GLFW_MOD_ALT) != 0;
        UI::handleKeyboard(leftaltKey, modAlt);
    }

    if (((mods & GLFW_MOD_SUPER) != 0) != modSuper) {
        modSuper = (mods & GLFW_MOD_SUPER) != 0;
        UI::handleKeyboard(leftguiKey, modSuper);
    }

    KeyboardButton b;
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            b = leftmouseKey;
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            b = rightmouseKey;
            break;

        case GLFW_MOUSE_BUTTON_MIDDLE:
            b = middlemouseKey;
            break;

        default:
            b = unknownKey;
            break;
    }

    UI::handleMouseClick(lastMouseX, lastMouseY, b, (action == GLFW_RELEASE));
}

void WindowGLFW::scrollCallback(GLFWwindow* w, double xoffset, double yoffset) {
    UI::handleMouseScroll(xoffset, yoffset);
}

KeyboardButton WindowGLFW::convertAsciiButton(int key) {
    // Alphanumerics can be returned as is
    if ((key >= '0') && (key <= '9')) {
        return static_cast<KeyboardButton>(key);
    } else if ((key >= 'A') && (key <= 'Z')) {
        key = key - 'A' + 'a';
        return static_cast<KeyboardButton>(key);
    }

    //! \fixme GLFW requires keyboard layout? Currently US is hard coded
    switch (key) {
        case ' ':
            return spaceKey;

        case '!':
            return oneKey;

        case '@':
            return twoKey;

        case '#':
            return threeKey;

        case '$':
            return fourKey;

        case '%':
            return fiveKey;

        case '^':
            return sixKey;

        case '&':
            return sevenKey;

        case '*':
            return eightKey;

        case '(':
            return nineKey;

        case ')':
            return zeroKey;

        case '"':
        case '\'':
            return quoteKey;

        case '+':
        case '=':
            return equalsKey;

        case ',':
        case '<':
            return commaKey;

        case '-':
        case '_':
            return minusKey;

        case '.':
        case '>':
            return dotKey;

        case '/':
        case '?':
            return slashKey;

        case ':':
        case ';':
            return semicolonKey;

        case '[':
        case '{':
            return leftbracketKey;

        case ']':
        case '}':
            return rightbracketKey;

        case '\\':
        case '|':
            return backslashKey;

        case '`':
        case '~':
            return backquoteKey;

        case GLFW_KEY_TAB:
            return tabKey;

        case GLFW_KEY_BACKSPACE:
            return backspaceKey;

        case GLFW_KEY_ENTER:
            return enterKey;

        case GLFW_KEY_ESCAPE:
            return escapeKey;

        case GLFW_KEY_F1:
            return f1Key;

        case GLFW_KEY_F2:
            return f2Key;

        case GLFW_KEY_F3:
            return f3Key;

        case GLFW_KEY_F4:
            return f4Key;

        case GLFW_KEY_F5:
            return f5Key;

        case GLFW_KEY_F6:
            return f6Key;

        case GLFW_KEY_F7:
            return f7Key;

        case GLFW_KEY_F8:
            return f8Key;

        case GLFW_KEY_F9:
            return f9Key;

        case GLFW_KEY_F10:
            return f10Key;

        case GLFW_KEY_F11:
            return f11Key;

        case GLFW_KEY_F12:
            return f12Key;

        case GLFW_KEY_LEFT:
            return leftKey;

        case GLFW_KEY_UP:
            return upKey;

        case GLFW_KEY_RIGHT:
            return rightKey;

        case GLFW_KEY_DOWN:
            return downKey;

        case GLFW_KEY_PAGE_UP:
            return pageupKey;

        case GLFW_KEY_PAGE_DOWN:
            return pagedownKey;

        case GLFW_KEY_HOME:
            return homeKey;

        case GLFW_KEY_END:
            return endKey;

        case GLFW_KEY_INSERT:
            return insertKey;

        default:
            return unknownKey;
    }
}

