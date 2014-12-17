/*!
 * \file src/WindowGLFW.cpp
 * \brief GLFW windowing implementation
 *
 * \author xythobuz
 */

#include <cstring>

#include "global.h"
#include "Log.h"
#include "RunTime.h"
#include "UI.h"
#include "utils/strings.h"
#include "system/WindowGLFW.h"

static int lastMouseX = 0;
static int lastMouseY = 0;

WindowGLFW::WindowGLFW() {
    mInit = false;
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    mFullscreen = false;
    mMousegrab = false;
    mTextInput = false;
    mWindow = nullptr;
}

WindowGLFW::~WindowGLFW() {
    if (mInit) {
        if (mWindow) {
            glfwDestroyWindow(mWindow);
        }

        glfwTerminate();
    }
}

int WindowGLFW::initialize() {
    assert(mInit == false);

    glfwSetErrorCallback(WindowGLFW::errorCallback);
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mWindow = glfwCreateWindow(mWidth, mHeight, VERSION,
                               mFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    if (!mWindow) {
        glfwTerminate();
        return -2;
    }

    glfwMakeContextCurrent(mWindow);

    glfwSetWindowSizeCallback(mWindow, WindowGLFW::sizeCallback);
    glfwSetCursorPosCallback(mWindow, WindowGLFW::cursorCallback);
    glfwSetKeyCallback(mWindow, WindowGLFW::keyCallback);
    glfwSetMouseButtonCallback(mWindow, WindowGLFW::buttonCallback);
    glfwSetScrollCallback(mWindow, WindowGLFW::scrollCallback);

    mInit = true;
    return 0;
}

void WindowGLFW::eventHandling() {
    assert(mInit == true);

    glfwPollEvents();

    if (glfwWindowShouldClose(mWindow)) {
        getRunTime().setRunning(false);
    }

    UI::eventsFinished();
}

void WindowGLFW::setSize(unsigned int width, unsigned int height) {
    assert(width > 0);
    assert(height > 0);

    if (mInit) {
        if ((mWidth != width) || (mHeight != height)) {
            glfwSetWindowSize(mWindow, width, height);
            getWindow().resizeGL();
        }
    }

    mWidth = width;
    mHeight = height;
}

void WindowGLFW::setFullscreen(bool fullscreen) {
    mFullscreen = fullscreen;

    //! \todo GLFW does not support toggling fullscreen?!
}

void WindowGLFW::setMousegrab(bool grab) {
    mMousegrab = grab;

    if (mInit == true) {
        if (mMousegrab)
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void WindowGLFW::setTextInput(bool on) {
    assert(mInit == true);
    mTextInput = on;
}

void WindowGLFW::swapBuffersGL() {
    assert(mInit == true);
    glfwSwapBuffers(mWindow);
}

void WindowGLFW::errorCallback(int error, const char* desc) {
    getLog() << "GLFW Error (" << error << "): " << desc << Log::endl;
}

void WindowGLFW::sizeCallback(GLFWwindow* w, int width, int height) {
    getWindow().setSize(width, height);
}

void WindowGLFW::cursorCallback(GLFWwindow* w, double xpos, double ypos) {
    int xrel = xpos - lastMouseX;
    int yrel = ypos - lastMouseY;
    UI::handleMouseMotion(xrel, yrel, xpos, ypos);
    lastMouseX = xpos;
    lastMouseY = ypos;
}

static bool modShift = false;
static bool modControl = false;
static bool modAlt = false;
static bool modSuper = false;

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

    if (getWindow().getTextInput() && (action != GLFW_RELEASE)) {
        //! \todo Handle text input properly!
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

