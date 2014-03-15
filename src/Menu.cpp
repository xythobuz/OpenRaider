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
    mCursor = 0;

    mainText.text = bufferString(VERSION);
    mainText.color[0] = 0xFF;
    mainText.color[1] = 0xFF;
    mainText.color[2] = 0xFF;
    mainText.color[3] = 0xFF;
    mainText.scale = 1.2f;
    mainText.y = 10;
    mainText.w = 0;
    mainText.h = 0;

    tempText.text = new char[256];
    tempText.color[0] = 0xFF;
    tempText.color[1] = 0xFF;
    tempText.color[2] = 0xFF;
    tempText.color[3] = 0xFF;
    tempText.scale = 1.2f;
    tempText.w = 0;
    tempText.h = 0;
}

Menu::~Menu() {
    delete [] mainText.text;
    delete [] tempText.text;
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
    vsnprintf(tempText.text, 256, s, args);
    tempText.text[255] = '\0';
    va_end(args);

    tempText.scale = scale;
    tempText.x = x;
    tempText.y = y;
    gOpenRaider->mWindow->writeString(&tempText);
}

void Menu::displayMapList() {
    // Estimate displayable number of items
    int items = (gOpenRaider->mWindow->mHeight - 110) / 25;

    // Select which part of the list to show
    int min, max;
    if (((int)mCursor - (items / 2)) > 0)
        min = mCursor - (items / 2);
    else
        min = 0;

    if ((mCursor + (items / 2)) < gOpenRaider->mMapList.size())
        max = mCursor + (items / 2);
    else
        max = gOpenRaider->mMapList.size();

    while ((max - min) < items) {
        if (min > 0)
            min--;
        else if (max < (gOpenRaider->mMapList.size()))
            max++;
        else
            break;
    }

    for (int i = 0; i < (max - min); i++) {
        char *map = gOpenRaider->mMapList[i + min];
        if ((i + min) == mCursor) {
            // Less greem & red --> highlight in red
            tempText.color[1] = 0x42;
            tempText.color[2] = 0x42;
        } else {
            tempText.color[1] = 0xFF;
            tempText.color[2] = 0xFF;
        }
        drawText(25, 100 + (25 * i), 0.75f, "%s", map);
    }
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
        window->writeString(&mainText);

        if (!gOpenRaider->mMapListFilled) {
            drawText(25, (window->mHeight / 2) - 20, 0.75f, "Generating map list...");
        } else {
            displayMapList();
        }
    }
}

void Menu::handleKeyboard(KeyboardButton key, bool pressed) {
    if (!pressed)
        return;

    if (key == up) {
        if (mCursor > 0)
            mCursor--;
        else
            mCursor = gOpenRaider->mMapList.size() - 1;
    } else if (key == down) {
        if (mCursor < (gOpenRaider->mMapList.size() - 1))
            mCursor++;
        else
            mCursor = 0;
    } else if (key == right) {
        int i = 10;
        if (mCursor > (gOpenRaider->mMapList.size() - 11))
            i = gOpenRaider->mMapList.size() - 1 - mCursor;
        while (i-- > 0)
            handleKeyboard(down, true);
    } else if (key == left) {
        int i = 10;
        if (mCursor < 10)
            i = mCursor;
        while (i-- > 0)
            handleKeyboard(up, true);
    } else if (key == enter) {

    }
}

