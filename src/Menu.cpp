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
#include "Font.h"
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
    hiddenState = false;
    dialogState = false;
}

Menu::~Menu() {
    delete mapFolder;
    mapFolder = nullptr;
}

int Menu::initialize() {
    return initialize(getOpenRaider().mPakDir);
}

int Menu::initialize(std::string folder) {
    return initialize(new Folder(folder, hiddenState));
}

int Menu::initialize(Folder *folder) {
    if (mapFolder != nullptr)
        delete mapFolder;
    mapFolder = folder;
    mMin = mCursor = 0;

    mapFolder->executeRemoveFiles([](File &f) {
        // Filter files based on file name
        if ((f.getName().length() > 4)
            && (f.getName().compare(f.getName().length() - 4, 4, ".phd") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".tr2") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".tr4") != 0)
            && (f.getName().compare(f.getName().length() - 4, 4, ".trc") != 0)) {
            return true; // delete file from list
        }

        // Check maps for validity
        int error = TombRaider::checkMime(f.getPath().c_str());
        if (error != 0) {
            getConsole() << "Error: pak file '" << f.getName().c_str()
                << "' " << ((error == -1) ? "not found" : "invalid") << Console::endl;
            return true; // delete file from list
        }

        return false; // keep file on list
    });

    if ((mapFolder->fileCount() + mapFolder->folderCount()) > 0)
        mCursor = 1; // Don't select ".." by default

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

    // Draw half-transparent overlay
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glDisable(GL_TEXTURE_2D);
    glRecti(0, 0, (GLint)getWindow().getWidth(), (GLint)getWindow().getHeight());
    glEnable(GL_TEXTURE_2D);

    // Draw heading
    getFont().drawTextCentered(0, 10, 1.2f, BLUE, getWindow().getWidth(), VERSION);

    // Estimate displayable number of items
    int items = (getWindow().getHeight() - 60) / 25;

    // Print list of "..", folders, files
    for (long i = mMin; (i < (mMin + items))
                && (i < (mapFolder->folderCount() + mapFolder->fileCount() + 1)); i++) {
        if (i == 0) {
            getFont().drawText(25, 50, 0.75f, (mCursor == i) ? RED : BLUE, "..");
        } else {
            getFont().drawText(25, (unsigned int)(50 + (25 * (i - mMin))), 0.75f,
                (mCursor == i) ? RED : BLUE,
                ((i - 1) < mapFolder->folderCount()) ?
                    (mapFolder->getFolder(i - 1).getName() + "/")
                    : mapFolder->getFile(i - 1 - mapFolder->folderCount()).getName());
        }
    }

    displayDialog();
}

void Menu::loadOrOpen() {
    if (mCursor == 0) {
        if (initialize(mapFolder->getParent().getPath()) != 0) {
            showDialog("Error reading parent folder!", "OK", "");
        }
    } else if ((mCursor - 1) < mapFolder->folderCount()) {
        if (initialize(mapFolder->getFolder(mCursor - 1).getPath()) != 0) {
            showDialog("Error reading subfolder!", "OK", "");
        }
    } else {
        std::string tmp = "load ";
        tmp += mapFolder->getFile((unsigned long)mCursor - 1 - mapFolder->folderCount()).getPath();
        if (getOpenRaider().command(tmp.c_str()) == 0) {
            setVisible(false);
        } else {
            showDialog("Error loading map!", "OK", "");
        }
    }
}

void Menu::handleKeyboard(KeyboardButton key, bool pressed) {
    if (!pressed)
        return;

    if (dialogText.length() > 0) {
        if (dialogButton2.length() == 0) {
            if (key == enterKey) {
                ackDialog();
            }
        } else {
            if (key == enterKey) {
                ackDialog();
            } else if (key == leftKey) {
                dialogState = !dialogState;
            } else if (key == rightKey) {
                dialogState = !dialogState;
            } else if (key == upKey) {
                dialogState = !dialogState;
            } else if (key == downKey) {
                dialogState = !dialogState;
            }
        }
        return;
    }

    assert(mapFolder != nullptr);
    int items = (getWindow().getHeight() - 60) / 25;

    if (key == upKey) {
        if (mCursor > 0)
            mCursor--;
        else
            mCursor = (long)(mapFolder->folderCount() + mapFolder->fileCount());
    } else if (key == downKey) {
        if (mCursor < (long)(mapFolder->folderCount() + mapFolder->fileCount()))
            mCursor++;
        else
            mCursor = 0;
    } else if (key == enterKey) {
        loadOrOpen();
    } else if (key == dotKey) {
        hiddenState = !hiddenState;
        initialize(mapFolder->getPath());
    }

    if (mCursor > (mMin + items - 1)) {
        mMin = mCursor - items + 1;
    } else if (mCursor < mMin) {
        mMin = mCursor;
    }
}

void Menu::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    int items = (getWindow().getHeight() - 60) / 25;

    if (released || (button != leftmouseKey))
        return;

    if (dialogText.length() > 0) {
        //!< \todo Allow mouse usage of Menu dialogs!
        return;
    }

    if ((y >= 50) && (y <= (unsigned int)(50 + (25 * items)))
            && ((mMin + (y / 25)) <= (mapFolder->folderCount() + mapFolder->fileCount() + 2))) {
        y -= 50;
        if (mCursor == (mMin + (y / 25)))
            loadOrOpen();
        else
            mCursor = mMin + (y / 25);
    }
}

void Menu::handleMouseScroll(int xrel, int yrel) {
    if (dialogText.length() > 0)
        return;

    assert((xrel != 0) || (yrel != 0));
    assert(mapFolder != nullptr);
    int items = (getWindow().getHeight() - 60) / 25;

    if ((mapFolder->folderCount() + mapFolder->fileCount() + 1) > items) {
        if (yrel < 0) {
            if (mMin < (mapFolder->folderCount() + mapFolder->fileCount() + 1 - items))
                mMin++;
        } else if (yrel > 0) {
            if (mMin > 0)
                mMin--;
        }

        if (mCursor < mMin) {
            mCursor = mMin;
        } else if (mCursor > (mMin + items - 1)) {
            mCursor = mMin + items - 1;
        }
    }
}

void Menu::showDialog(std::string msg, std::string btn1, std::string btn2,
        std::function<int (bool state)> callback) {
    // Only show one dialog at a time
    assert(dialogText.length() == 0);
    assert(dialogButton1.length() == 0);
    assert(dialogButton2.length() == 0);

    assert(msg.length() > 0);
    assert(btn1.length() > 0);

    dialogText = msg;
    dialogButton1 = btn1;
    dialogButton2 = btn2;
    dialogState = false;
    dialogFunction = callback;

    getConsole() << dialogText << Console::endl;
}

void Menu::ackDialog() {
    dialogText = "";
    dialogButton1 = "";
    dialogButton2 = "";

    if (dialogFunction) {
        int error = dialogFunction(dialogState);
        if (error != 0) {
            showDialog("Error processing dialog callback!", "OK", "");
        }
    }

    dialogState = false;
}

void Menu::displayDialog() {
    if (dialogText.length() > 0) {
        unsigned int wMax = ((unsigned int)(getWindow().getWidth() * 0.66f));

        unsigned int w0 = getFont().widthText(1.0f, dialogText) + 20;
        if (w0 > wMax)
            w0 = wMax;
        unsigned int h0 =  getFont().heightText(1.0f, w0, dialogText) + 10;

        assert(dialogButton1.length() > 0);
        unsigned int w1 = getFont().widthText(1.0f, dialogButton1) + 20;
        if (w1 > wMax)
            w1 = wMax;
        unsigned int h1 = getFont().heightText(1.0f, w1, dialogButton1) + 10;

        unsigned int wOverlay = wMax, hOverlay, w2 = 0, h2 = 0;

        if (dialogButton2.length() > 0) {
            // Show text and two buttons
            w2 = getFont().widthText(1.0f, dialogButton2) + 20;
            if (w2 > wMax)
                w2 = wMax;
            h2 = getFont().heightText(1.0f, w2, dialogButton2) + 10;

            if (w0 > (w1 + w2)) {
                if (w0 < wMax) {
                    wOverlay = w0;
                }
            } else if (w0 < (w1 + w2)) {
                if ((w1 + w2) < wMax) {
                    wOverlay = (w1 + w2);
                }
            }

            if ((w1 + w2) <= wMax) {
                hOverlay = h0 + ((h1 + h2) / 2);
            } else {
                hOverlay = h0 + h1 + h2;
            }
        } else {
            // Show text and one button
            if (w0 > w1) {
                if (w0 < wMax) {
                    wOverlay = w0;
                }
            } else if (w0 < w1) {
                if (w1 < wMax) {
                    wOverlay = w1;
                }
            }

            hOverlay = h0 + h1;
        }

        unsigned int xOverlay = (getWindow().getWidth() - wOverlay) / 2;
        unsigned int yOverlay = (getWindow().getHeight() - hOverlay) / 2;

        glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
        glDisable(GL_TEXTURE_2D);
        glRecti(xOverlay, yOverlay, xOverlay + wOverlay, yOverlay + hOverlay);
        glEnable(GL_TEXTURE_2D);

        getFont().drawTextWrapped(xOverlay + 10, yOverlay + 5, 1.0f, BLUE, w0, dialogText);
        if (dialogButton2.length() > 0) {
            if ((w1 + w2) <= wMax) {
                getFont().drawTextWrapped(xOverlay + 10, yOverlay + 10 + h0, 1.0f,
                    dialogState ? BLUE : RED, w1, dialogButton1);
                getFont().drawTextWrapped(xOverlay + 10 + w1, yOverlay + 10 + h0,
                    1.0f, dialogState ? RED : BLUE, w2, dialogButton2);
            } else {
                getFont().drawTextWrapped((getWindow().getWidth() - w1) / 2,
                    yOverlay + 10 + h0, 1.0f, dialogState ? BLUE : RED, w1, dialogButton1);
                getFont().drawTextWrapped((getWindow().getWidth() - w2) / 2,
                    yOverlay + 10 + h0 + h1, 1.0f, dialogState ? RED : BLUE, w2, dialogButton2);
            }
        } else {
            getFont().drawTextWrapped((getWindow().getWidth() - w1) / 2,
                    yOverlay + 10 + h0, 1.0f, RED, w1, dialogButton1);
        }
    }
}

