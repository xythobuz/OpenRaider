/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <cstdio>
#include <cstring>

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "Menu.h"
#include "Render.h"
#include "Sound.h"
#include "TextureManager.h"
#include "TombRaider.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "Window.h"
#include "OpenRaider.h"

OpenRaider::OpenRaider() {
    mRunning = false;
    mFPS = false;
    mBaseDir = NULL;
    mPakDir = NULL;
    mAudioDir = NULL;
    mDataDir = NULL;

    for (int i = 0; i < ActionEventCount; i++)
        keyBindings[i] = unknownKey;
}

OpenRaider::~OpenRaider() {
    delete mBaseDir;
    mBaseDir = NULL;

    delete mPakDir;
    mPakDir = NULL;

    delete mAudioDir;
    mAudioDir = NULL;

    delete mDataDir;
    mDataDir = NULL;
}

int OpenRaider::initialize() {
    // Initialize Windowing
    int error = getWindow().initialize();
    if (error != 0) {
        printf("Could not initialize Window (%d)!\n", error);
        return -1;
    }

    // Initialize OpenGL
    error = getWindow().initializeGL();
    if (error != 0) {
        printf("Could not initialize OpenGL (%d)!\n", error);
        return -2;
    }

    // Initialize Font
    error = getFont().initialize();
    if (error != 0) {
        printf("Could not initialize Font (%d)!\n", error);
        return -3;
    }

    // Initialize Sound
    error = getSound().initialize();
    if (error != 0) {
        printf("Could not initialize Sound (%d)!\n", error);
        return -4;
    }

    // Initialize Texture Manager
    error = getTextureManager().initialize();
    if (error != 0) {
        printf("Could not initialize Textures (%d)!\n", error);
        return -5;
    }

    // Initialize game engine
    error = getGame().initialize();
    if (error != 0) {
        printf("Could not initialize Game (%d)!\n", error);
        return -6;
    }

    // Initialize main menu
    error = getMenu().initialize();
    if (error != 0) {
        printf("Could not initialize Menu (%d)!\n", error);
        return -7;
    }

#ifdef DEBUG
    mFPS = true;
#endif

    getMenu().setVisible(true);
    systemTimerReset();

    return 0;
}

void OpenRaider::run() {
    assert(mRunning == false);
    mRunning = true;
    while (mRunning)
        frame();
}

void OpenRaider::frame() {
    assert(mRunning == true);

    static clock_t fpsSum = 0, fpsCount = 0;
    static int fps = 0;
    clock_t startTime = systemTimerGet();

    // Get keyboard and mouse input
    getWindow().eventHandling();

    // Draw game scene
    getRender().display();

    // Draw 2D overlays (console and menu)
    getWindow().glEnter2D();

    getConsole().display();
    getMenu().display();

    // Draw FPS counter
    if (mFPS)
        getFont().drawText(10, getWindow().getHeight() - 20, 0.5f, BLUE, "%dFPS", fps);

#ifdef DEBUG
    // Draw debug infos
    if (getGame().isLoaded() && (!getMenu().isVisible())) {
        for (int i = 0; i < 3; i++) {
            getFont().drawText(10, getWindow().getHeight() - ((4 - i) * 20), 0.5f, BLUE, "%.2f (%.2f)",
                getGame().getLara().getPos(i) / 256.0f, getGame().getLara().getAngle(i));
        }
    }
#endif

    getWindow().glExit2D();

    // Put new frame on screen
    getWindow().swapBuffersGL();

    // Calculate FPS display value
    fpsCount++;
    fpsSum += (systemTimerGet() - startTime);
    if (fpsSum >= 250) {
        // Update every 250ms
        fps = (int)((float)fpsCount * (1000.0f / (float)fpsSum));
        fpsCount = fpsSum = 0;
    }
}

void OpenRaider::handleKeyboard(KeyboardButton key, bool pressed) {
    assert(key < unknownKey);
    assert(mRunning == true);

    if ((keyBindings[menuAction] == key) && pressed) {
        getMenu().setVisible(!getMenu().isVisible());
    } else if (!getMenu().isVisible()) {
        if ((keyBindings[consoleAction] == key) && pressed) {
            getConsole().setVisible(!getConsole().isVisible());
        } else if (!getConsole().isVisible()) {
            for (int i = forwardAction; i < ActionEventCount; i++) {
                if (keyBindings[i] == key) {
                    getGame().handleAction((ActionEvents)i, !pressed);
                }
            }
        } else {
            getConsole().handleKeyboard(key, pressed);
        }
    } else {
        getMenu().handleKeyboard(key, pressed);
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void OpenRaider::handleText(char *text, bool notFinished) {
    assert(text != NULL);
    assert(text[0] != '\0');
    assert(mRunning == true);

    if ((getConsole().isVisible()) && (!getMenu().isVisible())) {
        getConsole().handleText(text, notFinished);
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void OpenRaider::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    assert(button < unknownKey);
    assert(mRunning == true);

    if (getMenu().isVisible()) {
        getMenu().handleMouseClick(x, y, button, released);
    } else if (!getConsole().isVisible()) {
        for (int i = forwardAction; i < ActionEventCount; i++) {
            if (keyBindings[i] == button) {
                getGame().handleAction((ActionEvents)i, released);
            }
        }
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void OpenRaider::handleMouseMotion(int xrel, int yrel) {
    assert((xrel != 0) || (yrel != 0));
    assert(mRunning == true);

    if ((!getConsole().isVisible()) && (!getMenu().isVisible())) {
        getGame().handleMouseMotion(xrel, yrel);
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void OpenRaider::handleMouseScroll(int xrel, int yrel) {
    assert((xrel != 0) || (yrel != 0));
    assert(mRunning == true);

    if ((getConsole().isVisible()) && (!getMenu().isVisible())) {
        getConsole().handleMouseScroll(xrel, yrel);
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

