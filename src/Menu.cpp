/*!
 * \file src/Menu.cpp
 * \brief Menu 'overlay'
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
#include "Menu.h"
#include "utils/strings.h"

Menu::Menu() {
    mVisible = false;
    mainText.text = bufferString(VERSION);
    mainText.color[0] = 0xFF;
    mainText.color[1] = 0xFF;
    mainText.color[2] = 0xFF;
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

void Menu::display() {
    Window *window = gOpenRaider->mWindow;

    if (mVisible) {
        mainText.x = (window->mWidth / 2) - (mainText.w / 2);
        mainText.y = 10;
        window->writeString(&mainText);
    }
}

