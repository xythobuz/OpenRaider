/*!
 * \file src/Menu.cpp
 * \brief Menu 'overlay'
 *
 * \author xythobuz
 */

#include <cctype>

#ifndef WIN32
#include <dirent.h>
#endif

#include "global.h"
#include "Console.h"
#include "Menu.h"
#include "OpenRaider.h"
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

    mMapListFilled = false;
    mFirstPass = false;
}

Menu::~Menu() {
    delete [] mainText.text;

    while (mMapList.size() > 0) {
        delete [] mMapList.back();
        mMapList.pop_back();
    }
}

void Menu::setVisible(bool visible) {
    mVisible = visible;
}

bool Menu::isVisible() {
    return mVisible;
}

#ifdef WIN32
void Menu::loadPakFolderHelper(std::vector<char *> &list) {
    WIN32_FIND_DATA fd;
    char *tmp = bufferString("%s\\*", list.at(0));
    HANDLE hFind = FindFirstFile(tmp, &fd);
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            list.push_back(bufferString("%s\\%s", list.at(0), fd.cFileName));
        } else {
            char *fullPathMap = bufferString("%s\\%s", list.at(0), fd.cFileName);

            char *lowerPath = bufferString("%s", fullPathMap);
            for (char *p = lowerPath; *p; ++p) *p = (char)tolower(*p);

            // Check for valid extension
            if (stringEndsWith(lowerPath, ".phd")
                 || stringEndsWith(lowerPath, ".tr2")
                 || stringEndsWith(lowerPath, ".tr4")
                 || stringEndsWith(lowerPath, ".trc")) {
                int error = TombRaider::checkMime(fullPathMap);
                if (error == 0) {
                    // Just load relative filename
                    mMapList.push_back(bufferString("%s", (fullPathMap + strlen(getOpenRaider().mPakDir) + 1)));
                } else {
                    getConsole().print("Error: pak file '%s' %s",
                            fullPathMap, (error == -1) ? "not found" : "invalid");
                }
            }

            delete [] lowerPath;
            delete [] fullPathMap;
        }
    } while (FindNextFile(hFind, &fd) != 0);
    FindClose(hFind);
    delete [] tmp;
    delete [] list.at(0);
    list.erase(list.begin());
}
#endif

void Menu::loadPakFolderRecursive(const char *dir) {
    assert(dir != NULL);
    assert(dir[0] != '\0');
#ifdef WIN32
    std::vector<char *> list;
    list.push_back(bufferString("%s", dir));
    do {
        loadPakFolderHelper(list);
    } while (list.size() > 0);
#else
    struct dirent entry;
    struct dirent *ep = NULL;
    DIR *pakDir;

    pakDir = opendir(dir);
    if (pakDir != NULL) {
        readdir_r(pakDir, &entry, &ep);
        while (ep != NULL) {
            if (ep->d_type == DT_DIR) {
                if ((strcmp(".", ep->d_name) != 0)
                 && (strcmp("..", ep->d_name) != 0)) {
                    char *tmp = bufferString("%s%s", dir, ep->d_name);
                    char *next = fullPath(tmp, '/');
                    loadPakFolderRecursive(next);
                    delete next;
                    delete tmp;
                }
            } else {
                char *fullPathMap = bufferString("%s%s", dir, ep->d_name);

                char *lowerPath = bufferString("%s", fullPathMap);
                for (char *p = lowerPath; *p; ++p) *p = (char)tolower(*p);

                // Check for valid extension
                if (stringEndsWith(lowerPath, ".phd")
                     || stringEndsWith(lowerPath, ".tr2")
                     || stringEndsWith(lowerPath, ".tr4")
                     || stringEndsWith(lowerPath, ".trc")) {
                    int error = TombRaider::checkMime(fullPathMap);
                    if (error == 0) {
                        // Just load relative filename
                        mMapList.push_back(bufferString("%s", (fullPathMap + strlen(getOpenRaider().mPakDir) + 1)));
                    } else {
                        getConsole().print("Error: pak file '%s' %s",
                                fullPathMap, (error == -1) ? "not found" : "invalid");
                    }
                }

                delete [] lowerPath;
                delete [] fullPathMap;
            }
            readdir_r(pakDir, &entry, &ep);
        }
        closedir(pakDir);
    } else {
        getConsole().print("Could not open PAK dir %s!", dir);
    }
#endif
}

void Menu::fillMapList() {
    char *tmp = fullPath(getOpenRaider().mPakDir, '/');
    loadPakFolderRecursive(tmp);
    delete [] tmp;
    mMapListFilled = true;
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

    if ((mCursor + (items / 2)) < mMapList.size())
        max = mCursor + (items / 2);
    else
        max = mMapList.size();

    while ((max - min) < items) {
        if (min > 0)
            min--;
        else if (max < ((int)mMapList.size()))
            max++;
        else
            break;
    }

    mMin = min;

    for (int i = 0; i < (max - min); i++) {
        char *map = mMapList[i + min];
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

        if (!mMapListFilled) {
            getWindow().drawText(25, (getWindow().mHeight / 2) - 20, 0.75f, OR_BLUE, "Generating map list...");
        } else {
            if (mMapList.size() == 0) {
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

        // Fill map list after first render pass,
        // so menu *loading screen* is visible
        if (mFirstPass) {
            if (!mMapListFilled) {
                fillMapList();
            }
        } else {
            mFirstPass = true;
        }
    }
}

void Menu::play() {
    char *tmp = bufferString("load %s", mMapList[mCursor]);
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
            mCursor = mMapList.size() - 1;
    } else if (key == downKey) {
        if (mCursor < (mMapList.size() - 1))
            mCursor++;
        else
            mCursor = 0;
    } else if (key == rightKey) {
        int i = 10;
        if (mCursor > (mMapList.size() - 11))
            i = mMapList.size() - 1 - mCursor;
        while (i-- > 0)
            handleKeyboard(downKey, true);
    } else if (key == leftKey) {
        int i = 10;
        if (mCursor < 10)
            i = mCursor;
        while (i-- > 0)
            handleKeyboard(upKey, true);
    } else if (key == enterKey) {
        play();
    }
}

void Menu::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    int items = (getWindow().mHeight - 110) / 25;

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

