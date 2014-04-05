/*!
 * \file src/Menu.cpp
 * \brief Menu 'overlay'
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "config.h"
#include "global.h"
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
    int items = (getWindow().mHeight - 110) / 25;

    // Select which part of the list to show
    int min, max;
    if (((int)mCursor - (items / 2)) > 0)
        min = mCursor - (items / 2);
    else
        min = 0;

    if ((mCursor + (items / 2)) < getOpenRaider().mMapList.size())
        max = mCursor + (items / 2);
    else
        max = getOpenRaider().mMapList.size();

    while ((max - min) < items) {
        if (min > 0)
            min--;
        else if (max < ((int)getOpenRaider().mMapList.size()))
            max++;
        else
            break;
    }

    mMin = min;

    for (int i = 0; i < (max - min); i++) {
        char *map = getOpenRaider().mMapList[i + min];
        if ((i + min) == (int)mCursor)
            getWindow().drawText(25, 100 + (25 * i), 0.75f, RED, "%s", map);
        else
            getWindow().drawText(25, 100 + (25 * i), 0.75f, OR_BLUE, "%s", map);
    }
}

void Menu::display() {
    if (mVisible) {
        // Draw half-transparent *overlay*
        glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
        glDisable(GL_TEXTURE_2D);
        glRecti(0, 0, getWindow().mWidth, getWindow().mHeight);
        glEnable(GL_TEXTURE_2D);

        // Draw heading text
        mainText.x = (getWindow().mWidth / 2) - (mainText.w / 2);
        getWindow().writeString(&mainText);

        if (!getOpenRaider().mMapListFilled) {
            getWindow().drawText(25, (getWindow().mHeight / 2) - 20, 0.75f, OR_BLUE, "Generating map list...");
        } else {
            if (getOpenRaider().mMapList.size() == 0) {
                getWindow().drawText(25, (getWindow().mHeight / 2) - 20, 0.75f, RED, "No maps found! See README.md");
            } else {
                // draw *play button* above list
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glDisable(GL_TEXTURE_2D);
                glRecti(25, 25, 100, 75);
                glEnable(GL_TEXTURE_2D);
                getWindow().drawText(40, 35, 0.75f, BLACK, "Play");

                displayMapList();
            }
        }
    }
}

void Menu::handleKeyboard(KeyboardButton key, bool pressed) {
    if (!pressed)
        return;

    if (key == upKey) {
        if (mCursor > 0)
            mCursor--;
        else
            mCursor = getOpenRaider().mMapList.size() - 1;
    } else if (key == downKey) {
        if (mCursor < (getOpenRaider().mMapList.size() - 1))
            mCursor++;
        else
            mCursor = 0;
    } else if (key == rightKey) {
        int i = 10;
        if (mCursor > (getOpenRaider().mMapList.size() - 11))
            i = getOpenRaider().mMapList.size() - 1 - mCursor;
        while (i-- > 0)
            handleKeyboard(downKey, true);
    } else if (key == leftKey) {
        int i = 10;
        if (mCursor < 10)
            i = mCursor;
        while (i-- > 0)
            handleKeyboard(upKey, true);
    } else if (key == enterKey) {
        char *tmp = bufferString("load %s", getOpenRaider().mMapList[mCursor]);
        if (getOpenRaider().command(tmp) == 0) {
            setVisible(false);
        } else {
            //! \todo Display something if an error occurs
        }
        delete [] tmp;
    }
}

void Menu::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    int items = (getWindow().mHeight - 110) / 25;

    if ((!released) || (button != leftmouseKey))
        return;

    if ((y >= 100) && (y <= (unsigned int)(100 + (25 * items)))) {
        y -= 100;
        mCursor = mMin + (y / 25);
    } else if ((y >= 25) && (y <= 100) && (x >= 25) && (x <= 125)) {
        // Play button
        mCursor = 0;
    }
}

