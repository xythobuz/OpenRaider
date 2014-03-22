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
    mCursor = 0;
    mMin = 0;

    mainText.text = bufferString(VERSION);
    mainText.color[0] = 0xFF;
    mainText.color[1] = 0xFF;
    mainText.color[2] = 0xFF;
    mainText.color[3] = 0xFF;
    mainText.scale = 1.2f;
    mainText.y = 10;
    mainText.w = 0;
    mainText.h = 0;
}

Menu::~Menu() {
    delete [] mainText.text;
}

void Menu::setVisible(bool visible) {
    mVisible = visible;
}

bool Menu::isVisible() {
    return mVisible;
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
        else if (max < ((int)gOpenRaider->mMapList.size()))
            max++;
        else
            break;
    }

    mMin = min;

    for (int i = 0; i < (max - min); i++) {
        char *map = gOpenRaider->mMapList[i + min];
        unsigned char color[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        if ((i + min) == (int)mCursor) {
            // Less greem & red --> highlight in red
            color[1] = 0x42;
            color[2] = 0x42;
        }
        gOpenRaider->mWindow->drawText(25, 100 + (25 * i), 0.75f, color, "%s", map);
    }
}

void Menu::display() {
    Window *window = gOpenRaider->mWindow;
    unsigned char color[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    if (mVisible) {
        // Draw half-transparent *overlay*
        glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
        glDisable(GL_TEXTURE_2D);
        glRecti(0, 0, window->mWidth, window->mHeight);
        glEnable(GL_TEXTURE_2D);

        // Draw heading text
        mainText.x = (window->mWidth / 2) - (mainText.w / 2);
        window->writeString(&mainText);

        if (!gOpenRaider->mMapListFilled) {
            gOpenRaider->mWindow->drawText(25, (window->mHeight / 2) - 20, 0.75f, color, "Generating map list...");
        } else {
            if (gOpenRaider->mMapList.size() == 0) {
                gOpenRaider->mWindow->drawText(25, (window->mHeight / 2) - 20, 0.75f, color, "No maps found! See README.md");
            } else {
                // draw *play button* above list
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glDisable(GL_TEXTURE_2D);
                glRecti(25, 25, 100, 75);
                glEnable(GL_TEXTURE_2D);
                color[0] = color[1] = color[2] = 0x00;
                gOpenRaider->mWindow->drawText(40, 35, 0.75f, color, "Play");
                color[0] = color[1] = color[2] = 0xFF;

                displayMapList();
            }
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
        char *tmp = bufferString("load %s", gOpenRaider->mMapList[mCursor]);
        if (gOpenRaider->command(tmp) == 0)
            setVisible(false);
        delete [] tmp;
    }
}

void Menu::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    int items = (gOpenRaider->mWindow->mHeight - 110) / 25;

    if ((!released) || (button != leftmouse))
        return;

    if ((y >= 100) && (y <= (unsigned int)(100 + (25 * items)))) {
        y -= 100;
        mCursor = mMin + (y / 25);
    } else if ((y >= 25) && (y <= 100) && (x >= 25) && (x <= 125)) {
        // Play button
        mCursor = 0;
    }
}

