/*!
 * \file src/WindowGLUT.cpp
 * \brief GLUT windowing implementation
 *
 * \author xythobuz
 */

#include <cstring>

#include <GL/freeglut.h>

#include "global.h"
#include "RunTime.h"
#include "UI.h"
#include "utils/strings.h"
#include "WindowGLUT.h"

//! \todo Modifier keys currently don't create keyboard events...

static int lastMouseX = 0;
static int lastMouseY = 0;

WindowGLUT::WindowGLUT() {
    mInit = false;
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    mFullscreen = false;
    mMousegrab = false;
    mTextInput = false;
}

void WindowGLUT::setSize(unsigned int width, unsigned int height) {
    assert(width > 0);
    assert(height > 0);

    mWidth = width;
    mHeight = height;

    if (mInit == true) {
        glutReshapeWindow(width, height);
    }
}

void WindowGLUT::setFullscreen(bool fullscreen) {
    mFullscreen = fullscreen;

    if (mInit == true) {
        if (mFullscreen)
            glutFullScreen();
        else
            glutLeaveFullScreen();
    }
}

void WindowGLUT::setMousegrab(bool grab) {
    mMousegrab = grab;

    if (mInit == true) {
        if (mMousegrab)
            glutSetCursor(GLUT_CURSOR_NONE);
        else
            glutSetCursor(GLUT_CURSOR_INHERIT);
    }
}

int WindowGLUT::initialize() {
    assert(mInit == false);

    int argc = 1;
    char* argv[] = { new char[11], nullptr };
    strcpy(argv[0], "OpenRaider");
    glutInit(&argc, argv);
    glutInitWindowSize(mWidth, mHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow(VERSION);

    glutReshapeFunc(WindowGLUT::reshapeCallback);
    glutKeyboardFunc(WindowGLUT::keyboardCallback);
    glutKeyboardUpFunc(WindowGLUT::keyboardUpCallback);
    glutSpecialFunc(WindowGLUT::specialCallback);
    glutSpecialUpFunc(WindowGLUT::specialUpCallback);
    glutMouseFunc(WindowGLUT::mouseCallback);
    glutMotionFunc(WindowGLUT::motionCallback);
    glutPassiveMotionFunc(WindowGLUT::motionCallback);
    glutMouseWheelFunc(WindowGLUT::mouseWheelCallback);

    delete [] argv[0];

    mInit = true;
    return 0;
}

void WindowGLUT::eventHandling() {
    assert(mInit == true);

    glutMainLoopEvent();

    UI::eventsFinished();
}

void WindowGLUT::setTextInput(bool on) {
    assert(mInit == true);
    mTextInput = on;
}

void WindowGLUT::swapBuffersGL() {
    assert(mInit == true);
    glutSwapBuffers();
}

void WindowGLUT::reshapeCallback(int width, int height) {
    getWindow().resizeGL();
}

// Note that the escape, backspace, and delete keys are generated as an ASCII character.
void WindowGLUT::keyboardCallback(unsigned char key, int x, int y) {
    if (getWindow().getTextInput()) {
        if ((key >= ' ') && (key <= '~')) {
            char s[2] = { static_cast<char>(key), '\0' };
            UI::handleText(s, false);
        }
    }

    KeyboardButton b = convertAsciiButton(key);
    UI::handleKeyboard(b, true);
}

void WindowGLUT::keyboardUpCallback(unsigned char key, int x, int y) {
    KeyboardButton b = convertAsciiButton(key);
    UI::handleKeyboard(b, false);
}

void WindowGLUT::specialCallback(int key, int x, int y) {
    KeyboardButton b = convertKeyCode(key);
    UI::handleKeyboard(b, true);
}

void WindowGLUT::specialUpCallback(int key, int x, int y) {
    KeyboardButton b = convertKeyCode(key);
    UI::handleKeyboard(b, false);
}

void WindowGLUT::mouseCallback(int button, int state, int x, int y) {
    KeyboardButton b;

    switch (button) {
        case GLUT_LEFT_BUTTON:
            b = leftmouseKey;
            break;

        case GLUT_RIGHT_BUTTON:
            b = rightmouseKey;
            break;

        case GLUT_MIDDLE_BUTTON:
            b = middlemouseKey;
            break;

        default:
            b = unknownKey;
            break;
    }

    UI::handleMouseClick(x, y, b, (state == GLUT_UP));
}

// The x and y callback parameters indicate the mouse location in window relative coordinates.
void WindowGLUT::motionCallback(int x, int y) {
    int xrel = x - lastMouseX;
    int yrel = y - lastMouseY;
    UI::handleMouseMotion(xrel, yrel, x, y);
    lastMouseX = x;
    lastMouseY = y;

    if (getWindow().getMousegrab()) {
        lastMouseX = getWindow().getWidth() / 2;
        lastMouseY = getWindow().getHeight() / 2;
        glutWarpPointer(lastMouseX, lastMouseY);
    }
}

void WindowGLUT::mouseWheelCallback(int wheel, int direction, int x, int y) {
    int xrel = 0, yrel = 0;

    if (wheel == 0)
        yrel = direction;
    else
        xrel = direction;

    UI::handleMouseScroll(xrel, yrel);
}

KeyboardButton WindowGLUT::convertAsciiButton(unsigned char key) {
    // Convert Uppercase to Lowercase
    if ((key >= 'A') && (key <= 'Z'))
        key = key - 'A' + 'a';

    // Alphanumerics can be returned as is
    if (((key >= '0') && (key <= '9'))
        || ((key >= 'a') && (key <= 'z'))) {
        return static_cast<KeyboardButton>(key);
    }

    //! \fixme GLUT requires keyboard layout? Currently US is hard coded
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

        case '\t':
            return tabKey;

        case 8: // Backspace
            return backspaceKey;

        case '\r':
        case '\n':
            return enterKey;

        case 27: // Escape
            return escapeKey;

        default:
            return unknownKey;
    }
}

KeyboardButton WindowGLUT::convertKeyCode(int key) {
    switch (key) {
        case GLUT_KEY_F1:
            return f1Key;

        case GLUT_KEY_F2:
            return f2Key;

        case GLUT_KEY_F3:
            return f3Key;

        case GLUT_KEY_F4:
            return f4Key;

        case GLUT_KEY_F5:
            return f5Key;

        case GLUT_KEY_F6:
            return f6Key;

        case GLUT_KEY_F7:
            return f7Key;

        case GLUT_KEY_F8:
            return f8Key;

        case GLUT_KEY_F9:
            return f9Key;

        case GLUT_KEY_F10:
            return f10Key;

        case GLUT_KEY_F11:
            return f11Key;

        case GLUT_KEY_F12:
            return f12Key;

        case GLUT_KEY_LEFT:
            return leftKey;

        case GLUT_KEY_UP:
            return upKey;

        case GLUT_KEY_RIGHT:
            return rightKey;

        case GLUT_KEY_DOWN:
            return downKey;

        case GLUT_KEY_PAGE_UP:
            return pageupKey;

        case GLUT_KEY_PAGE_DOWN:
            return pagedownKey;

        case GLUT_KEY_HOME:
            return homeKey;

        case GLUT_KEY_END:
            return endKey;

        case GLUT_KEY_INSERT:
            return insertKey;

        default:
            return unknownKey;
    }
}

