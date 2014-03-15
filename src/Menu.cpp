/*!
 * \file src/Menu.cpp
 * \brief Menu 'overlay'
 *
 * \author xythobuz
 */

#include <cstdarg>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "config.h"
#include "main.h"
#include "Menu.h"
#include "utils/strings.h"

Menu::Menu() {
    mVisible = false;
    mainText.text = bufferString(VERSION);
    mainText.color[0] = 0xFF;
    mainText.color[1] = 0xFF;
    mainText.color[2] = 0xFF;
    mainText.color[3] = 0xFF;
    mainText.scale = 1.2f;
    mainText.w = 0;
    mainText.h = 0;
}

Menu::~Menu() {

}

void Menu::setVisible(bool visible) {
    mVisible = visible;
}

bool Menu::isVisible() {
    return mVisible;
}

void Menu::drawText(unsigned int x, unsigned int y, float scale, const char *s, ...) {
    va_list args;
    va_start(args, s);
    WindowString w;
    w.text = bufferString(s, args);
    va_end(args);
    w.scale = scale;
    w.x = x;
    w.y = y;
    w.color[0] = 0xFF;
    w.color[1] = 0xFF;
    w.color[2] = 0xFF;
    w.color[3] = 0xFF;
    gOpenRaider->mWindow->writeString(&w);
}

void Menu::display() {
    Window *window = gOpenRaider->mWindow;

    if (mVisible) {
        // Draw half-transparent *overlay*
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glDisable(GL_TEXTURE_2D);
        glRecti(0, 0, window->mWidth, window->mHeight);
        glEnable(GL_TEXTURE_2D);

        // Draw heading text
        mainText.x = (window->mWidth / 2) - (mainText.w / 2);
        mainText.y = 10;
        window->writeString(&mainText);

        drawText(20, (window->mHeight / 2) - 20, 0.75f, "Generating map list...");
    }
}

void Menu::actionMouse(unsigned int x, unsigned int y, int button) {

}

void Menu::actionKeyboard(int key) {

}

