/*!
 * \file src/Console.cpp
 * \brief Console 'overlay'
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "config.h"
#include "Console.h"
#include "main.h"
#include "utils/time.h"

Console::Console() {
    mVisible = false;
}

Console::~Console() {
}

void Console::setVisible(bool visible) {
    mVisible = visible;
    gOpenRaider->mWindow->setTextInput(mVisible);
}

bool Console::isVisible() {
    return mVisible;
}

void Console::display() {
    Window *window = gOpenRaider->mWindow;
    unsigned char color[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    if (mVisible) {
        // Draw half-transparent *overlay*
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glDisable(GL_TEXTURE_2D);
        glRecti(0, 0, window->mWidth, window->mHeight / 2);
        glEnable(GL_TEXTURE_2D);

        gOpenRaider->mWindow->drawText(10, 10, 0.50f, color, "%lus uptime %s", systemTimerGet() / 1000, VERSION);
    }
}

void Console::handleKeyboard(KeyboardButton key, bool pressed) {
    if (pressed && (key == enter)) {

    }
}

void Console::handleText(char *text, bool notFinished) {
    printf("Got %s (%s)\n", text, (notFinished ? "not finished" : "finished"));
}

