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
#include "main.h"
#include "Console.h"
#include "utils/strings.h"

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

        gOpenRaider->mWindow->drawText(25, (window->mHeight / 4) - 20, 0.75f, color, "Console");
    }
}

void Console::handleKeyboard(KeyboardButton key, bool pressed) {

}

void Console::handleText(char *text, bool notFinished) {
    printf("Got %s (%s)\n", (notFinished ? "not finished" : "finished"));
}

