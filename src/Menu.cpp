/*!
 * \file src/Menu.cpp
 * \brief Menu 'overlay'
 *
 * \author xythobuz
 */

#include <cctype>
#include <cstring>

#include "global.h"
#include "Console.h"
#include "OpenRaider.h"
#include "utils/strings.h"
#include "TombRaider.h"
#include "Window.h"
#include "Menu.h"


Menu::Menu() {
    mVisible = false;
    mCursor = 0;
    mMin = 0;
    mapFolder = nullptr;

    mainText.text = bufferString("%s", VERSION);
    mainText.color[0] = BLUE[0];
    mainText.color[1] = BLUE[1];
    mainText.color[2] = BLUE[2];
    mainText.color[3] = BLUE[3];
    mainText.scale = 1.2f;
    mainText.y = 10;
    mainText.w = 0;
    mainText.h = 0;
}

Menu::~Menu() {
    delete [] mainText.text;
    mainText.text = nullptr;

    delete mapFolder;
    mapFolder = nullptr;
}

int Menu::initialize() {
    mapFolder = new Folder(getOpenRaider().mPakDir);

    mapFolder->executeRemoveRecursiveItems([](File &f) {
        if ((f.getName().compare(f.getName().length() - 4, 4, ".phd") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".tr2") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".tr4") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".trc") != 0)) {
            return true;
        }
        int error = TombRaider::checkMime(f.getPath().c_str());
        if (error != 0) {
            getConsole().print("Error: pak file '%s' %s",
                    f.getName().c_str(), (error == -1) ? "not found" : "invalid");
            return true;
        }
        return false;
    });

    return 0;
}

void Menu::setVisible(bool visible) {
    mVisible = visible;
}

bool Menu::isVisible() {
    return mVisible;
}

void Menu::display() {
    if (!mVisible)
        return;

    // Draw half-transparent *overlay*
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glDisable(GL_TEXTURE_2D);
    glRecti(0, 0, getWindow().getWidth(), getWindow().getHeight());
    glEnable(GL_TEXTURE_2D);

    // Draw heading text, using FontString so we can get the
    // width of the drawn text to center it
    mainText.x = (getWindow().getWidth() / 2) - (mainText.w / 2);
    getFont().writeString(mainText);

    if ((mapFolder == nullptr) || (mapFolder->countRecursiveItems() == 0)) {
        getFont().drawText(25, (getWindow().getHeight() / 2) - 20, 0.75f, RED, "No maps found! See README.md");
        return;
    } else {
        // draw *play button* above list
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glDisable(GL_TEXTURE_2D);
        glRecti(25, 25, 100, 75);
        glEnable(GL_TEXTURE_2D);
        getFont().drawText(40, 35, 0.75f, BLACK, "Play");

        // Estimate displayable number of items
        int items = (getWindow().getHeight() - 110) / 25;

        // Select which part of the list to show
        long min, max;
        if (((long)mCursor - (items / 2)) > 0)
            min = mCursor - (items / 2);
        else
            min = 0;

        if ((mCursor + (items / 2)) < mapFolder->countRecursiveItems())
            max = mCursor + (items / 2);
        else
            max = mapFolder->countRecursiveItems();

        while ((max - min) < items) {
            if (min > 0)
                min--;
            else if (max < ((int)mapFolder->countRecursiveItems()))
                max++;
            else
                break;
        }

        mMin = min;

        for (int i = 0; i < (max - min); i++) {
            const char *map = mapFolder->getRecursiveItemName(i + min).c_str();
            if ((i + min) == (int)mCursor)
                getFont().drawText(25, 100 + (25 * i), 0.75f, RED, "%s", map);
            else
                getFont().drawText(25, 100 + (25 * i), 0.75f, BLUE, "%s", map);
        }
    }
}

void Menu::play() {
    char *tmp = bufferString("load %s", mapFolder->getRecursiveItemName(mCursor).c_str());
    getConsole().print("%s", tmp);
    if (getOpenRaider().command(tmp) == 0) {
        setVisible(false);
    } else {
        //! \todo Display something if an error occurs
    }
    delete [] tmp;
}

void Menu::handleKeyboard(KeyboardButton key, bool pressed) {
    if (!pressed)
        return;

    if (key == upKey) {
        if (mCursor > 0)
            mCursor--;
        else
            mCursor = mapFolder->countRecursiveItems() - 1;
    } else if (key == downKey) {
        if (mCursor < (mapFolder->countRecursiveItems() - 1))
            mCursor++;
        else
            mCursor = 0;
    } else if (key == rightKey) {
        long i = 10;
        if (mCursor > (mapFolder->countRecursiveItems() - 11))
            i = mapFolder->countRecursiveItems() - 1 - mCursor;
        while (i-- > 0)
            handleKeyboard(downKey, true);
    } else if (key == leftKey) {
        long i = 10;
        if (mCursor < 10)
            i = mCursor;
        while (i-- > 0)
            handleKeyboard(upKey, true);
    } else if (key == enterKey) {
        play();
    }
}

void Menu::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    int items = (getWindow().getHeight() - 110) / 25;

    if (released || (button != leftmouseKey))
        return;

    if ((y >= 100) && (y <= (unsigned int)(100 + (25 * items)))) {
        y -= 100;
        mCursor = mMin + (y / 25);
    } else if ((y >= 25) && (y <= 100) && (x >= 25) && (x <= 125)) {
        // Play button
        play();
    }
}

