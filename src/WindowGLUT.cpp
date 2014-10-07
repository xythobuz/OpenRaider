/*!
 * \file src/WindowGLUT.cpp
 * \brief GLUT windowing implementation
 *
 * \author xythobuz
 */

#include <cstdio>
#include <ctime>

#include "global.h"
#include "RunTime.h"
#include "UI.h"
#include "utils/strings.h"
#include "WindowGLUT.h"

WindowGLUT::WindowGLUT() {
    mInit = false;
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    mFullscreen = false;
    mMousegrab = false;
    mTextInput = false;
}

WindowGLUT::~WindowGLUT() {
    if (mInit) {
    }
}

void WindowGLUT::setSize(unsigned int width, unsigned int height) {
    assert(width > 0);
    assert(height > 0);

    mWidth = width;
    mHeight = height;

    if (mInit == true) {
        resizeGL();
    }
}

void WindowGLUT::setFullscreen(bool fullscreen) {
    mFullscreen = fullscreen;

    if (mInit == true) {
    }
}

void WindowGLUT::setMousegrab(bool grab) {
    mMousegrab = grab;

    if (mInit == true) {
    }
}

bool WindowGLUT::getMousegrab() {
    return mMousegrab;
}

int WindowGLUT::initialize() {
    assert(mInit == false);


    mInit = true;


    return 0;
}

void WindowGLUT::eventHandling() {
    assert(mInit == true);



    UI::eventsFinished();
}

void WindowGLUT::setTextInput(bool on) {
    assert(mInit == true);

    mTextInput = on;
}

bool WindowGLUT::getTextInput() {
    assert(mInit == true);
    return mTextInput;
}

void WindowGLUT::delay(unsigned int ms) {
    assert(mInit == true);
}

void WindowGLUT::swapBuffersGL() {
    assert(mInit == true);
}

