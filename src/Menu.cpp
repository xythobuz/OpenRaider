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
    if (mapFolder != nullptr)
        delete mapFolder;
    mapFolder = new Folder(getOpenRaider().mPakDir);

    mapFolder->executeRemoveRecursiveItems([](File &f) {
        // Filter files based on file name
        if ((f.getName().compare(f.getName().length() - 4, 4, ".phd") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".tr2") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".tr4") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".trc") != 0)) {
            return true; // delete file from list
        }

        // Check maps for validity
        int error = TombRaider::checkMime(f.getPath().c_str());
        if (error != 0) {
            getConsole().print("Error: pak file '%s' %s",
                    f.getName().c_str(), (error == -1) ? "not found" : "invalid");
            return true; // delete file from list
        }

        return false; // keep file on list
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
    glRecti(0, 0, (GLint)getWindow().getWidth(), (GLint)getWindow().getHeight());
    glEnable(GL_TEXTURE_2D);

    // Draw heading text, using FontString so we can get the
    // width of the drawn text to center it
    mainText.x = (getWindow().getWidth() / 2) - ((unsigned int)(mainText.w / 2));
    getFont().writeString(mainText);

    if ((mapFolder == nullptr) || (mapFolder->countRecursiveItems() == 0)) {
        getFont().drawText(25, (getWindow().getHeight() / 2) - 20, 0.75f, RED, "No maps found! See README.md");
        return;
    } else {
        // Estimate displayable number of items
        int items = (getWindow().getHeight() - 60) / 25;

        // Select which part of the list to show
        long min, max;
        if (((long)mCursor - (items / 2)) > 0)
            min = mCursor - (items / 2);
        else
            min = 0;

        if ((mCursor + (items / 2)) < (long)mapFolder->countRecursiveItems())
            max = mCursor + (items / 2);
        else
            max = (long)mapFolder->countRecursiveItems();

        while ((max - min) < items) {
            if (min > 0)
                min--;
            else if (max < ((int)mapFolder->countRecursiveItems()))
                max++;
            else
                break;
        }

        mMin = min;

        for (long i = 0; i < (max - min); i++) {
            const char *map = mapFolder->getRecursiveItemName((unsigned long)(i + min)).c_str();
            getFont().drawText(25, (unsigned int)(50 + (25 * i)), 0.75f,
                    ((i + min) == mCursor) ? RED : BLUE, "%s", map);
        }
    }
}

void Menu::play() {
    char *tmp = bufferString("load %s", mapFolder->getRecursiveItemName((unsigned long)mCursor).c_str());
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
            mCursor = (long)mapFolder->countRecursiveItems() - 1;
    } else if (key == downKey) {
        if (mCursor < (long)(mapFolder->countRecursiveItems() - 1))
            mCursor++;
        else
            mCursor = 0;
    } else if (key == rightKey) {
        long i = 10;
        if (mCursor > (long)(mapFolder->countRecursiveItems() - 11))
            i = ((long)mapFolder->countRecursiveItems()) - 1 - mCursor;
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
    int items = (getWindow().getHeight() - 60) / 25;

    if (released || (button != leftmouseKey))
        return;

    if ((y >= 50) && (y <= (unsigned int)(50 + (25 * items)))) {
        y -= 50;
        if (mCursor == (mMin + (y / 25)))
            play();
        else
            mCursor = mMin + (y / 25);
    }
}

